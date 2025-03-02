#ifndef ZYKLIO_ROSCONNECTOR_H
#define ZYKLIO_ROSCONNECTOR_H

#ifdef _WIN32
// Ensure that Winsock2.h is included before Windows.h, which can get
// pulled in by anybody (e.g., Boost).
#ifndef WINSOCK2_H
#define WINSOCK2_H
#include <Winsock2.h>
#endif
#endif

#include <boost/shared_ptr.hpp>

#include <ros/master.h>
#include <ros/node_handle.h>
#include <ros/topic.h>

#include <ros/callback_queue.h>

#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/core/objectmodel/Data.h>

#include "init_ZyROSConnector.h"
#include "ZyROSConnectorTopicSubscriber.h"
#include "ZyROSConnectorTopicPublisher.h"

#include "ZyROSConnectorServiceClient.h"
#include "ZyROSConnectorServiceServer.h"

#include <boost/thread/condition.hpp>

//#include <sofa/simulation/SimulationConcurrentComponent.h>

using namespace sofa::core::objectmodel;

namespace Zyklio
{
    namespace ROSConnector
    {
        // Data members in private implementation
        class ZyklioRosConnectorPrivate;
        class SOFA_ZY_ROS_CONNECTOR_API ZyROSConnector : public sofa::core::objectmodel::BaseObject
        {
            public:
                SOFA_CLASS(ZyROSConnector, sofa::core::objectmodel::BaseObject);

                ZyROSConnector();
                virtual ~ZyROSConnector();

                // SimulationConcurrentComponent API
                void startComponent();
                void stopComponent();
                void pauseComponent();
                void resumeComponent();

                bool isConnected() const;
                bool isThreadRunning() const;
                boost::condition& connectorCondition();

                bool setRosMasterURI(const std::string&);

                ros::NodeHandlePtr getROSNode();

                bool addTopicListener(const boost::shared_ptr<ZyROSListener> &);
                bool removeTopicListener(boost::shared_ptr<ZyROSListener>&);

                bool addTopicPublisher(boost::shared_ptr<ZyROSPublisher>&);
                bool removeTopicPublisher(boost::shared_ptr<ZyROSPublisher>&);

                bool addServiceClient(boost::shared_ptr<ZyROSServiceClient>&);
                bool removeServiceClient(boost::shared_ptr<ZyROSServiceClient>&);

                size_t getNumTopicListeners() const;
                size_t getNumTopicPublishers() const;
                size_t getNumServiceClients() const;

            protected:
                friend class ZyROSConnectorWorkerThread;

                boost::posix_time::ptime theTime;

                std::string m_rosMasterURI;

                void startConnector();
                void stopConnector();

                void rosLoop();

                ZyklioRosConnectorPrivate* m_d;

                ros::NodeHandlePtr m_rosNode;
                ros::CallbackQueue cb_queue;

                bool m_connectorThreadActive;
                boost::condition m_runCondition;
        };
    }
}

#endif //ZYKLIO_ROSCONNECTOR_H
