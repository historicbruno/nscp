/**************************************************************************
*   Copyright (C) 2004-2007 by Michael Medin <michael@medin.name>         *
*                                                                         *
*   This code is part of NSClient++ - http://trac.nakednuns.org/nscp      *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <pdh/pdh_query.hpp>

namespace PDH {

	PDHQuery::~PDHQuery(void) {
		removeAllCounters();
	}

	void PDHQuery::addCounter(pdh_instance instance) {
		if (instance->has_instances()) {
			BOOST_FOREACH(pdh_instance child, instance->get_instances()) {
				counters_.push_back(boost::make_shared<PDHCounter>(child));
			}
		} else 
			counters_.push_back(boost::make_shared<PDHCounter>(instance));
	}

	bool PDHQuery::has_counters() {
		return !counters_.empty();
	}


	void PDHQuery::removeAllCounters() {
		if (hQuery_)
			close();
		counters_.clear();
	}

	void PDHQuery::on_unload() {
		if (hQuery_ == NULL)
			return;
		BOOST_FOREACH(counter_type c, counters_) {
			c->remove();
		}
		pdh_error status = factory::get_impl()->PdhCloseQuery(hQuery_);
		if (status.is_error())
			throw pdh_exception("PdhCloseQuery failed", status);
		hQuery_ = NULL;
	}
	void PDHQuery::on_reload() {
		if (hQuery_ != NULL)
			return;
		pdh_error status = factory::get_impl()->PdhOpenQuery(NULL, 0, &hQuery_);
		if (status.is_error())
			throw pdh_exception("PdhOpenQuery failed", status);
		BOOST_FOREACH(counter_type c, counters_) {
			c->addToQuery(getQueryHandle());
		}
	}

	void PDHQuery::open() {
		if (hQuery_ != NULL)
			throw pdh_exception("query is not null!");
		factory::get_impl()->add_listener(this);
		on_reload();
	}

	void PDHQuery::close() {
		if (hQuery_ == NULL)
			throw pdh_exception("query is null!");
		factory::get_impl()->remove_listener(this);
		on_unload();
		counters_.clear();
	}

	void PDHQuery::gatherData(bool ignore_errors) {
		collect();
		BOOST_FOREACH(counter_type c, counters_) {
			pdh_error status = c->collect();
			if (status.is_negative_denominator()) {
				Sleep(500);
				collect();
				status = c->collect();
			}
			if (status.is_negative_denominator()) {
				if (!hasDisplayedInvalidCOunter_) {
					hasDisplayedInvalidCOunter_ = true;
					throw pdh_exception(c->getName() + " Negative denominator issue (check FAQ for ways to solve this): ", status);
				}
			} else if (!ignore_errors && status.is_error()) {
				throw pdh_exception(c->getName() + " Failed to poll counter " + c->get_path(), status);
			}
		}
	}
	void PDHQuery::collect() {
		pdh_error status = factory::get_impl()->PdhCollectQueryData(hQuery_);
		if (status.is_error())
			throw pdh_exception("PdhCollectQueryData failed: ", status);
	}

	PDH::PDH_HQUERY PDHQuery::getQueryHandle() const {
		return hQuery_;
	}
}