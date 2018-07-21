/*
* Copyright (c) 2017 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef IFDBCLI
#define IFDBCLI

#include <memory>
#include <cpr/cpr.h>
#include <base/common.h>
#include <base/threading/common/Thread.h>
#include <base/threading/dispatcher/DispatcherTypes.h>

using namespace std;
using namespace cpr;
using namespace base::threading;

namespace base { namespace metrics
{
	struct MetricUpdateDetails
	{
		public:
			std::string url;
			std::string value;
			MetricUpdateDetails(std::string URL, std::string Value)
			{
				url.assign(URL);
				value.assign(Value);
			}
	};
	class InfluxDBClientThread : public Thread
	{
		public:
			void Init() override {}

	};
	class InfluxDBClient
	{
		public:
			void UpdateMetricTask(void* details);
			void UpdateIntMetric(string name, int value);
			void UpdateDoubleMetric(string name, double value);
			void InitIFDBConnection(string serverAddress, string dbName, string appName);
		private:
			void UpdateMetricInternal(string name, string value);

			std::string app;
			std::string database;
			std::string dbServer;
			InfluxDBClientThread* senderThread;

		SINGLETON(InfluxDBClient)
	};
}}

#endif