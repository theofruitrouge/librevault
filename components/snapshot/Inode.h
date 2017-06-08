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
#pragma once
#include "EncryptedData.h"
#include "ChunkInfo.h"
#include <Secret.h>
#include <QByteArray>
#include <QDebug>
#include <QList>
#include <chrono>

namespace librevault {

namespace serialization {
class Inode;
}

class Inode {
public:
	enum InodeType {
		DELETED = 0,
		FILE = 1,
		DIRECTORY = 2,
		SYMLINK = 3
	};
	using Timestamp = std::chrono::nanoseconds;

private:
	struct InodeData : public QSharedData {
		EncryptedData path;
		Timestamp timestamp;
		InodeType inode_type;
		Timestamp mtime;
		quint64 mtime_granularity;
		quint32 windows_attrib;
		quint32 mode;
		quint32 uid;
		quint32 gid;
		QList<ChunkInfo> chunks;
		EncryptedData symlink_target;
	};

public:
	Inode() {d = new InodeData;}
	Inode(const serialization::Inode& serialized);
	operator serialization::Inode() const;

	EncryptedData path() const {return d->path;}
	void setPath(EncryptedData path) {d->path = path;}

	Timestamp timestamp() const {return d->timestamp;}
	void setTimestamp(Timestamp timestamp) {d->timestamp = timestamp;}

	InodeType inodeType() const {return d->inode_type;}
	void setInodeType(InodeType inode_type) {d->inode_type = inode_type;}

	Timestamp mtime() const {return d->mtime;}
	void setMtime(Timestamp mtime) {d->mtime = mtime;}

	quint64 mtimeGranularity() const {return d->mtime_granularity;}
	void setMtimeGranularity(quint64 mtime_granularity) {d->mtime_granularity = mtime_granularity;}

	quint32 windowsAttrib() const {return d->windows_attrib;}
	void setWindowsAttrib(quint32 windows_attrib) {d->windows_attrib = windows_attrib;}

	quint32 mode() const {return d->mode;}
	void setMode(quint32 mode) {d->mode = mode;}

	quint32 uid() const {return d->uid;}
	void setUid(quint32 uid) {d->uid = uid;}

	quint32 gid() const {return d->gid;}
	void setGid(quint32 gid) {d->gid = gid;}

	QList<ChunkInfo> chunks() const {return d->chunks;}
	void setChunks(QList<ChunkInfo> chunks) {d->chunks = chunks;}

	EncryptedData symlinkTarget() const {return d->symlink_target;}
	void setSymlinkTarget(EncryptedData symlink_target) {d->symlink_target = symlink_target;}

private:
	QSharedDataPointer<InodeData> d;
};

} /* namespace librevault */
