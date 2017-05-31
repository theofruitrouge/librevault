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
#include <Secret.h>
#include <QByteArray>
#include <QSharedData>
#include <QString>
#include <QList>
#include <chrono>

namespace librevault {

namespace serialization {
class InodeGeneric;
}

/* SnapshotEntry */
struct InodeGenericData : public QSharedData {
	std::chrono::nanoseconds mtime_ = std::chrono::nanoseconds(0);
	quint32 windows_attrib_ = 0;
	quint32 mode_ = 0;
	quint32 uid_ = 0;
	quint32 gid_ = 0;
};

class InodeGeneric {
public:
	InodeGeneric() {d_ = new InodeGenericData;}
	InodeGeneric(const serialization::InodeGeneric& serialized);
	operator serialization::InodeGeneric() const;

	std::chrono::nanoseconds mtime() const {return d_->mtime_;}
	void setMtime(std::chrono::nanoseconds path) {d_->mtime_ = path;}

	quint32 windowsAttrib() const {return d_->windows_attrib_;}
	void setWindowsAttrib(quint32 windows_attrib) {d_->windows_attrib_ = windows_attrib;}

	quint32 mode() const {return d_->mode_;}
	void setMode(quint32 mode) {d_->mode_ = mode;}

	quint32 uid() const {return d_->uid_;}
	void setUid(quint32 uid) {d_->uid_ = uid;}

	quint32 gid() const {return d_->gid_;}
	void setGid(quint32 gid) {d_->gid_ = gid;}

private:
	QSharedDataPointer<InodeGenericData> d_;
};

} /* namespace librevault */
