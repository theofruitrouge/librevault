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
#include <QList>
#include <QSharedData>

namespace librevault {

namespace serialization {
class Snapshot;
}

class Snapshot {
	struct SnapshotData : public QSharedData {
		qint64 revision;
		QList<QByteArray> inode_hash;
		quint32 max_chunksize;
		quint32 min_chunksize;
		quint64 polynomial;
		quint32 polynomial_degree;
		quint32 polynomial_shift;
		quint32 avg_bits;
	};

public:
	Snapshot() {d = new SnapshotData;}
	Snapshot(const serialization::Snapshot& serialized);
	operator serialization::Snapshot() const;

	qint64 revision() const {return d->revision;}
	void set_revision(qint64 revision) {d->revision = revision;}

	QList<QByteArray> inodeHash() {return d->inode_hash;}
	void setInodeHash(QList<QByteArray> inode_hash) {d->inode_hash = inode_hash;}

	quint32 maxChunksize() const {return d->max_chunksize;}
	void setMaxChunksize(quint32 max_chunksize) {d->max_chunksize = max_chunksize;}

	quint32 minChunksize() const {return d->min_chunksize;}
	void setMinChunksize(quint32 min_chunksize) {d->min_chunksize = min_chunksize;}

	quint64 polynomial() const {return d->polynomial;}
	void setPolynomial(quint64 polynomial) {d->polynomial = polynomial;}

	quint32 polynomialDegree() const {return d->polynomial_degree;}
	void setPolynomialDegree(quint32 polynomial_degree) {d->polynomial_degree = polynomial_degree;}

	quint32 polynomialShift() const {return d->polynomial_shift;}
	void setPolynomialShift(quint32 polynomial_shift) {d->polynomial_shift = polynomial_shift;}

	quint32 avgBits() const {return d->avg_bits;}
	void setAvgBits(quint32 avg_bits) {d->avg_bits = avg_bits;}

private:
	QSharedDataPointer<SnapshotData> d;
};

} /* namespace librevault */
