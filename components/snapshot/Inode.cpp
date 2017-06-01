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
#include "Inode.h"
#include <Snapshot.pb.h>

namespace librevault {

inline ::google::protobuf::Timestamp conv_timestamp(Inode::Timestamp timestamp) {
	::google::protobuf::Timestamp timestamp_pb;
	timestamp_pb.set_seconds(std::chrono::duration_cast<std::chrono::seconds>(timestamp).count());
	timestamp_pb.set_nanos(timestamp.count() % 1000000000ll);
	return timestamp_pb;
};

inline Inode::Timestamp conv_timestamp(::google::protobuf::Timestamp timestamp) {
	return Inode::Timestamp(timestamp.seconds()*1000000000ll + timestamp.nanos());
};

Inode::Inode(const serialization::Inode& serialized) {
	d = new InodeData;
	d->path = serialized.path();
	d->timestamp = conv_timestamp(serialized.timestamp());
	d->inode_type = (InodeType)serialized.type();
	d->mtime = conv_timestamp(serialized.mtime());
	d->mtime_granularity = serialized.mtime_granularity();
	d->windows_attrib = serialized.windows_attrib();
	d->mode = serialized.mode();
	d->uid = serialized.uid();
	d->gid = serialized.gid();
	d->chunks.reserve(serialized.chunks_size());
	for(const auto& chunk : serialized.chunks())
		d->chunks.push_back(chunk);
	d->symlink_target = serialized.symlink_target();
}

Inode::operator serialization::Inode() const {
	serialization::Inode serialized;
	(*serialized.mutable_path()) = d->path;
	(*serialized.mutable_timestamp()) = conv_timestamp(d->timestamp);
	serialized.set_type((serialization::Inode_InodeType)d->inode_type);
	(*serialized.mutable_mtime()) = conv_timestamp(d->mtime);
	serialized.set_mtime_granularity(d->mtime_granularity);
	serialized.set_windows_attrib(d->windows_attrib);
	serialized.set_mode(d->mode);
	serialized.set_uid(d->uid);
	serialized.set_gid(d->gid);
	for(const auto& chunk : d->chunks)
		(*serialized.add_chunks()) = chunk;
	(*serialized.mutable_symlink_target()) = d->symlink_target;
	return serialized;
}

} /* namespace librevault */
