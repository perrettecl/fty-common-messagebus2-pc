#include <cstring>
#include <iostream>
#include <memory>

#include "fty/messagebus/MsgBusAmqp.hpp"
#include "fty/messagebus/MsgBusMqtt.hpp"

#include <fty/messagebus/MsgBusMessage.hpp>

// #include <fty/messagebus/ContainerInterface.hpp>
// #include <fty/messagebus/mqtt/MsgBusMqttMessage.hpp>

using namespace fty::messagebus;
//using Message = fty::messagebus::MsgBusMessage;
using Message = fty::messagebus::mqttv5::MqttMessage;

/*
 * This code provides an example design pattern to achieve the following:
 *
 * We want to implement Container class using two different implementations
 * We further want to select its implementation during runtime
 * We also want to be able to do the same for any classes derived from Container
 * We even want to be able to templatize on Container class
 */

/**
 * interface that all implementations must bind to
 */
// class ContainerInterface {
// public:
//     virtual ~ContainerInterface() = default;
//     virtual void identify() const = 0;

//     virtual size_t size() const = 0;

// };

/**
 * one type of implementation
 */
// class MsgBusMqtt : public ContainerInterface<Message>
// {
// public:
//   MsgBusMqtt() = default;
//   void identify() const override
//   {
//     std::cout << "mqtt" << std::endl;
//   }

//   DeliveryState subscribe(const std::string& topic, MessageListener<Message> messageListener) override
//   {
//     std::cout << "mqtt subscribed" << std::endl;
//     return DeliveryState::DELI_STATE_UNAVAILABLE;
//   }

//   DeliveryState unsubscribe(const std::string& topic) override
//   {
//     return DeliveryState::DELI_STATE_UNAVAILABLE;
//   }

//   DeliveryState publish(const std::string& topic, const std::string& message) override
//   {
//     return DeliveryState::DELI_STATE_UNAVAILABLE;
//   }

//   DeliveryState sendRequest(const std::string& requestQueue, const std::string& message, MessageListener<Message> messageListener) override
//   {
//     return DeliveryState::DELI_STATE_UNAVAILABLE;
//   }

//   Opt<Message> sendRequest(const std::string& requestQueue, const std::string& message, int timeOut) override
//   {
//     Opt<Message> val{};
//     return val;
//   }

//   DeliveryState waitRequest(const std::string& requestQueue, MessageListener<Message> messageListener) override
//   {
//     return DeliveryState::DELI_STATE_UNAVAILABLE;
//   }

//   DeliveryState sendReply(const std::string& response, const Message& message) override
//   {
//     return DeliveryState::DELI_STATE_UNAVAILABLE;
//   }

// private:
//   /* ... */
// };

/**
 * another type of implementation
 */
// class MsgBusAmqp : public ContainerInterface<Message>
// {
//   using Message = fty::messagebus::mqttv5::MqttMessage;
// public:
//   MsgBusAmqp() = default;
//   void identify() const override
//   {
//     std::cout << "amqp" << std::endl;
//   }

//   DeliveryState subscribe(const std::string& topic, MessageListener<Message> messageListener) override
//   {
//     std::cout << "amqp subscribed" << std::endl;
//     return DeliveryState::DELI_STATE_UNAVAILABLE;
//   }

//   DeliveryState unsubscribe(const std::string& topic) override
//   {
//     return DeliveryState::DELI_STATE_UNAVAILABLE;
//   }

//   DeliveryState publish(const std::string& topic, const std::string& message) override
//   {
//     return DeliveryState::DELI_STATE_UNAVAILABLE;
//   }

//   DeliveryState sendRequest(const std::string& requestQueue, const std::string& message, MessageListener<Message> messageListener) override
//   {
//     return DeliveryState::DELI_STATE_UNAVAILABLE;
//   }

//   Opt<Message> sendRequest(const std::string& requestQueue, const std::string& message, int timeOut) override
//   {
//     Opt<Message> val{};
//     return val;
//   }

//   DeliveryState waitRequest(const std::string& requestQueue, MessageListener<Message> messageListener) override
//   {
//     return DeliveryState::DELI_STATE_UNAVAILABLE;
//   }

//   DeliveryState sendReply(const std::string& response, const Message& message) override
//   {
//     return DeliveryState::DELI_STATE_UNAVAILABLE;
//   }

// private:
//   /* ... */
// };

/**
 * a concrete container taking one implementation
 */
template <typename MessageType>
class Container : public ContainerInterface<MessageType>
{
public:
  explicit Container(std::unique_ptr<ContainerInterface<MessageType>> impl)
    : _impl{std::move(impl)}
  {
  }
  std::string identify() const override
  {
    return _impl->identify();
  }

  DeliveryState subscribe(const std::string& topic, MessageListener<MessageType> messageListener) override
  {
    return _impl->subscribe(topic, messageListener);
  }

  DeliveryState unsubscribe(const std::string& topic) override
  {
    return _impl->unsubscribe(topic);
  }

  DeliveryState publish(const std::string& topic, const std::string& message) override
  {
    return _impl->publish(topic, message);
  }

  DeliveryState sendRequest(const std::string& requestQueue, const std::string& message, MessageListener<MessageType> messageListener) override
  {
    return _impl->sendRequest(requestQueue, message, messageListener);
  }

  Opt<MessageType> sendRequest(const std::string& requestQueue, const std::string& message, int timeOut) override
  {
    return _impl->sendRequest(requestQueue, message, timeOut);
  }

  DeliveryState waitRequest(const std::string& requestQueue, MessageListener<MessageType> messageListener) override
  {
    return _impl->waitRequest(requestQueue, messageListener);
  }

  DeliveryState sendReply(const std::string& response, const MessageType& message) override
  {
    return _impl->sendReply(response, message);
  }

private:
  std::unique_ptr<ContainerInterface<MessageType>> _impl;
};

/**
 * Demonstration of templated private inheritance
 * Queue is implemented through Container APIs
 */
//template <typename C = Container<MessageType>>

template <
  typename MessageType,
  typename C = Container<MessageType>>
class MsgBus : private C
{
public:
  explicit MsgBus(std::unique_ptr<ContainerInterface<MessageType>> impl)
    : Container<MessageType>{std::move(impl)}
  {
    C::identify();
  }

  DeliveryState subscribe(const std::string& topic, MessageListener<MessageType> messageListener)
  {
    return C::subscribe(topic, messageListener);
  }

  DeliveryState unsubscribe(const std::string& topic)
  {
    return C::unsubscribe(topic);
  }

  DeliveryState publish(const std::string& topic, const std::string& message)
  {
    return C::publish(topic, message);
  }

  DeliveryState sendRequest(const std::string& requestQueue, const std::string& message, MessageListener<MessageType> messageListener)
  {
    return C::sendRequest(requestQueue, message, messageListener);
  }

  Opt<MessageType> sendRequest(const std::string& requestQueue, const std::string& message, int timeOut)
  {
    return C::sendRequest(requestQueue, message, timeOut);
  }

  DeliveryState waitRequest(const std::string& requestQueue, MessageListener<MessageType> messageListener)
  {
    return C::waitRequest(requestQueue, messageListener);
  }

  DeliveryState sendReply(const std::string& response, const MessageType& message)
  {
    return C::sendReply(response, message);
  }
};

/**
 * Demonstration of public inheritance
 * AdvancedContainer extends Container APIs
 * but its implementations should only rely on Container APIs
 * and should not depend on implementation specific of array or list
 */
// class AdvancedContainer : public Container
// {
// public:
//   explicit AdvancedContainer(std::unique_ptr<ContainerInterface> impl)
//     : Container{std::move(impl)}
//   {
//   }
// };

int main(int argc, const char** argv)
{
  // two copies of implementations depending on user input, determined during run-time
  //std::unique_ptr<ContainerInterface<Message>> impl1, impl2;
  if (strcmp(argv[1], "mqtt") == 0)
  {

    auto msgBus2 = MsgBusMqtt();
    //ContainerInterface<MqttMessage> msgBus2 = MsgBusMqtt();
    msgBus2.publish("e", "azerty");

    //auto impl1 = std::unique_ptr<ContainerInterface<MqttMessage>>(new MsgBusMqtt);
    //auto impl1 = std::make_unique<ContainerInterface<MqttMessage>>(new MsgBusMqtt);


    // MsgBus<MqttMessage, Container<MqttMessage>> msgBus{std::move(impl1)};
    // msgBus.publish("e", "azerty");
    //msgBus.unsubscribe("e");
    //impl2 = std::unique_ptr<ContainerInterface<Message>>(new MsgBusMqtt);
  }
  else
  {
    // impl1 = std::unique_ptr<ContainerInterface<Message>>(new MsgBusAmqp);
    // impl2 = std::unique_ptr<ContainerInterface<Message>>(new MsgBusAmqp);
  }

  // both Queue and AdvancedContainer implementations are determined during run-time
  // MsgBus<Container> msgBus{std::move(impl1)};
  // msgBus.unsubscribe("e");
  // AdvancedContainer advancedContainer{std::move(impl2)};
  // advancedContainer.identify();
  return 0;
}
