/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "nb-socket-service.h"
#include "socket/network-api/posix/tcp/server-event-handler.h"
#include "socket/network-api/posix/tcp/event-manager.h"

using namespace std::placeholders;

namespace netty {
    namespace net {
        NBSocketService::~NBSocketService() {
            memory_barrier();
            if (!m_bStopped) {
                Stop();
            }

            DELETE_PTR(m_netStackWorkerManager);
            DELETE_PTR(m_pEventManager);
        }

        bool NBSocketService::Start(NonBlockingEventModel m) {
            m_bStopped = false;
            if (m_nlt.get()) {
                if (SocketProtocal::Tcp == m_nlt->sp) {
                    m_pEventManager = new PosixTcpEventManager(&m_nlt->nat, m_pMemPool, MAX_EVENTS, (uint32_t)(common::CPUS_CNT / 2),
                                                                std::bind(&NBSocketService::on_connect, this, _1),
                                                               std::bind(&NBSocketService::on_finish, this, _1),
                                                               std::bind(&NBSocketService::check_handler_valid, this, _1));
                    m_pEventManager->Start(m);
                } else {
                    throw std::runtime_error("Not support now!");
                }
            }

            return 0;
        }

        bool NBSocketService::Stop() {
            m_bStopped = true;
            return m_pEventManager->Stop();
        }

        bool NBSocketService::Connect(net_peer_info_t &npt) {

        }

        bool NBSocketService::Disconnect(net_peer_info_t &npt) {

        }

        bool NBSocketService::SendMessage(SndMessage *m) {

        }

        void NBSocketService::on_connect(AFileEventHandler *handler) {
            if (m_netStackWorkerManager->PutWorkerEventHandler(handler->GetSocketDescriptor()->GetPeerInfo(), handler)) {
                m_pEventManager->AddEvent(handler, EVENT_NONE, EVENT_READ|EVENT_WRITE);
            } else {
                DELETE_PTR(handler);
            }
        }

        void NBSocketService::on_finish(AFileEventHandler *handler) {
            auto ew = handler->GetOwnWorker();
            ew->DeleteHandler(handler);
            m_netStackWorkerManager->ReleaseWorkerEventHandler(handler);
        }

        bool NBSocketService::check_handler_valid(AFileEventHandler *handler) {

        }
    } // namespace net
} // namespace netty
