/* Copyright (C) 2017 Alexander Shishenko <alex@shishenko.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */
#include "InodeScanner.h"
#include "TimestampGranularity.h"
#include <AES_CBC.h>
#include <PathNormalizer.h>
#include <rabin.h>
#include <QCryptographicHash>
#include <QFile>
#include <QLoggingCategory>
#include <boost/filesystem.hpp>
#ifdef Q_OS_UNIX
#   include <sys/stat.h>
#endif
#ifdef Q_OS_WIN
#   include <windows.h>
#endif

namespace fs = boost::filesystem;

namespace librevault {

Q_LOGGING_CATEGORY(log_inodescanner, "snapshot.inode.scanner")

InodeScanner::InodeScanner(QString path, Secret secret, QString root, Snapshot snapshot, IInodeStorage* inode_storage, QObject* parent) :
	QObject(parent), path_(path), secret_(secret), root_(root), snapshot_(snapshot), inode_storage_(inode_storage), stopping_(false) {}

void InodeScanner::run() {
	try {
		emit inodeCreated(createInode());
	}catch(std::exception& e) {
		emit inodeFailed(e.what());
	}
}

Inode InodeScanner::createInode() {
	Inode new_inode;
	new_inode.setPath(EncryptedData(secret_, PathNormalizer::normalizePath(path_, root_)));
	new_inode.setTimestamp(std::chrono::system_clock::now().time_since_epoch());
	new_inode.setInodeType(getType());

	if(new_inode.inodeType() != Inode::DELETED) {
		updateAttributes(new_inode);
		new_inode.setMtimeGranularity(timestampGranularity(path_).count());
	}

	if(new_inode.inodeType() == Inode::SYMLINK) {
		QString symlink_target = QString::fromStdWString(fs::read_symlink(path_.toStdWString()).wstring());
		new_inode.setSymlinkTarget(EncryptedData(secret_, PathNormalizer::normalizePath(symlink_target, root_)));
	}

	if(new_inode.inodeType() == Inode::FILE) {
		new_inode.setChunks(getChunks());
	}

	return new_inode;
}

Inode::InodeType InodeScanner::getType() const {
	fs::file_status file_status = fs::symlink_status(path_.toStdWString());

	switch(file_status.type()) {
		case fs::regular_file: return Inode::FILE;
		case fs::directory_file: return Inode::DIRECTORY;
		case fs::symlink_file: return Inode::SYMLINK;
		case fs::file_not_found: return Inode::DELETED;
		default: throw abort_index("File type is unsuitable for indexing. Only Files, Directories and Symbolic links are supported");
	}
}

void InodeScanner::updateAttributes(Inode& inode) const {
#if defined(Q_OS_UNIX)
	struct stat stat_buf;
	int stat_err = lstat(path_.toUtf8(), &stat_buf);

	if(stat_err != 0)
		throw abort_index("stat() failed during processing UNIX attributes");

	inode.setMode(stat_buf.st_mode);    // Mode
	inode.setUid(stat_buf.st_uid);      // Uid
	inode.setGid(stat_buf.st_gid);      // Gid

#   if defined(Q_OS_DARWIN)
	inode.setMtime(std::chrono::seconds(stat_buf.st_mtimespec.tv_sec) + std::chrono::nanoseconds(stat_buf.st_mtimespec.tv_nsec));
#   else
	inode.setMtime(std::chrono::seconds(stat_buf.st_mtim.tv_sec) + std::chrono::nanoseconds(stat_buf.st_mtim.tv_nsec));
#   endif
#endif

#if defined(Q_OS_WIN)
	new_inode.setWindowsAttrib(GetFileAttributes(path.toStdWString().c_str()));	// Windows attributes (I don't have Windows now to test it), this code is stub for now.
	inode.setMtime(std::chrono::seconds(fs::last_write_time(path.toStdWString()))); // Shim
#endif
}

QList<ChunkInfo> InodeScanner::getChunks() const {
	// Initializing chunker
	rabin_t hasher;
	hasher.average_bits = snapshot_.avgBits();
	hasher.minsize = snapshot_.minChunksize();
	hasher.maxsize = snapshot_.maxChunksize();
	hasher.polynomial = snapshot_.polynomial();
	hasher.polynomial_degree = snapshot_.polynomialDegree();
	hasher.polynomial_shift = snapshot_.polynomialShift();

	hasher.mask = uint64_t((1<<uint64_t(hasher.average_bits))-1);

	rabin_init(&hasher);

	QList<ChunkInfo> chunks;
	QHash<QByteArray, ChunkInfo> pt_keyed_hashes;

	QByteArray file_buffer;
	file_buffer.reserve(hasher.maxsize);

	QFile f(path_);
	if(!f.open(QIODevice::ReadOnly))
		throw abort_index("I/O error: " + f.errorString());

	char byte;
	while(f.getChar(&byte)) {
		file_buffer.push_back(byte);

		interruptionPoint();

		if(rabin_next_chunk(&hasher, (uchar*)&byte, 1) == 1) {    // Found a chunk
			chunks << populateChunk(file_buffer, pt_keyed_hashes);
			pt_keyed_hashes.insert(chunks.last().ptKeyedHash(), chunks.last());
			file_buffer.clear();
		}
	}

	interruptionPoint();

	if(rabin_finalize(&hasher) != 0)
		chunks << populateChunk(file_buffer, pt_keyed_hashes);

	return chunks;
}

ChunkInfo InodeScanner::populateChunk(const QByteArray& data, const QHash<QByteArray, ChunkInfo>& pt_keyed_hashes) const {
	ChunkInfo chunk;

	{
		QCryptographicHash hasher(QCryptographicHash::Sha3_256);
		hasher.addData(secret_.getEncryptionKey());
		hasher.addData(data);
		chunk.setPtKeyedHash(hasher.result());
	}

	// If we already have this chunk inside this inode
	if(pt_keyed_hashes.contains(chunk.ptKeyedHash())) {
		return pt_keyed_hashes[chunk.ptKeyedHash()];
	}

	// If we already have this chunk inside whole storage
	if(inode_storage_->haveChunkInfoByPtKeyedHash(chunk.ptKeyedHash())) {
		return inode_storage_->getChunkInfoByPtKeyedHash(chunk.ptKeyedHash());
	}

	chunk.setIv(generateRandomIV());
	chunk.setSize(data.size());

	{
		QCryptographicHash hasher(QCryptographicHash::Sha3_256);
		hasher.addData(encryptAesCbc(data, secret_.getEncryptionKey(), chunk.iv()));
		chunk.setCtHash(hasher.result());
	}

	return chunk;
}

void InodeScanner::interruptionPoint() const {
	if(stopping_)
		throw abort_index("Indexing had been interruped");
}

} /* namespace librevault */
