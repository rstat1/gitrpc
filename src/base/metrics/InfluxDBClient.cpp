/*
* Copyright (c) 2017 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <algorithm>
#include <base/common.h>
#include <base/metrics/InfluxDBClient.h>

namespace base { namespace metrics
{
	std::shared_ptr<InfluxDBClient> InfluxDBClient::ref;

	void InfluxDBClient::UpdateIntMetric(string name, int value)
	{
		this->UpdateMetricInternal(name, to_string(value));
	}
	void InfluxDBClient::UpdateDoubleMetric(string name, double value)
	{
		this->UpdateMetricInternal(name, to_string(value));
	}
	void InfluxDBClient::UpdateMetricInternal(std::string name, std::string value)
	{
		std::transform(name.begin(), name.end(), name.begin(), [](char c) {
			return c == ' ' ? '_' : c;
		});

		std::string postURL = this->dbServer;
		std::string metric;
		metric.append(name);
		metric.append(",platform=");
		metric.append(_Platform);
		metric.append(" ");
		metric.append("value=");
		metric.append(value);
		postURL.append("/write?db=").append(this->database);

		MetricUpdateDetails* details = new MetricUpdateDetails(postURL, metric);
		NEW_TASK1(UpdateMetric, InfluxDBClient, InfluxDBClient::Get(),
				  InfluxDBClient::UpdateMetricTask, details);
		POST_TASK(UpdateMetric, "MetricsSender");
	}
	void InfluxDBClient::UpdateMetricTask(void* args)
	{
		// MetricUpdateDetails* details = (MetricUpdateDetails*)args;
		// cpr::Response resp = cpr::Post(cpr::Url(details->url), Body{details->value});
		// if (resp.status_code != 204) { Log("MetricSender", "%s", resp.text.c_str()); }
	}
	void InfluxDBClient::InitIFDBConnection(string serverAddress, string dbName, string appName)
	{
		this->dbServer = serverAddress;
		this->database = dbName;
		this->app = appName;

		this->senderThread = new InfluxDBClientThread();
		this->senderThread->Start("MetricsSender");
	}
}}