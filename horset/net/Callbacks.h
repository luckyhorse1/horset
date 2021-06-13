#include <functional>
#include <memory>
#include "horset/base/Timestamp.h"

class TcpConnection;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void(const TcpConnectionPtr &)> ConnectionCallback;

typedef std::function<void(const TcpConnectionPtr &,
                           char *,
                           Timestamp)> MessageCallback;
