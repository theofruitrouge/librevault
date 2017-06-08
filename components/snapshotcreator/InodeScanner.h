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
#include "IChunkStorage.h"
#include "IInodeStorage.h"
#include "ISnapshotStorage.h"
#include <Inode.h>
#include <Snapshot.h>
#include <Secret.h>
#include <QList>
#include <QHash>
#include <QRunnable>
#include <QObject>

namespace librevault {

class InodeScanner : public QObject, public QRunnable {
	Q_OBJECT
signals:
	void inodeCreated(Inode inode);
	void inodeFailed(QString error_string);

public:
	struct abort_index : public std::runtime_error {
		abort_index(QString what) : std::runtime_error(what.toStdString()) {}
	};

	InodeScanner(QString path, Secret secret, QString root, Snapshot snapshot, IInodeStorage* inode_storage, QObject* parent = nullptr);

	Inode createInode();    // Synchronously scan the inode

	QString path() const {return path_;}

public slots:
	void run() override;    // Asynchronously scan the inode
	void stop() {stopping_ = true;} // Interrupt the scan (synchronous or asynchronous)

private:
	QString path_;
	Secret secret_;
	QString root_;
	Snapshot snapshot_; // Used for snapshot parameters
	IInodeStorage* inode_storage_;

	std::atomic<bool> stopping_;

	Inode::InodeType getType() const;
	void updateAttributes(Inode& inode) const;
	QList<ChunkInfo> getChunks() const;

	ChunkInfo populateChunk(const QByteArray& data, const QHash<QByteArray, ChunkInfo>& pt_keyed_hashes) const;

	void interruptionPoint() const;
};

} /* namespace librevault */
