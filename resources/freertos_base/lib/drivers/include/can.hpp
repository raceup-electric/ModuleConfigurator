#pragma once

#include <cstdint>
#include <optional>

#include "common/capability.hpp"
#include "common/common.hpp"

namespace ru::driver {

enum class CanBitrate: uint32_t{
  BR_125K = 125'000,
  BR_250K = 250'000,
  BR_500K = 500'000,
  BR_1M = 1'000'000
};

class CanConfig : public Config {
public:
  const CanId m_id;
  CanBitrate m_normal_bitrate;
  CanConfig(CanId id, CanBitrate bitrate);
};

class CanMessage {
public:
    uint32_t id;
    uint8_t  len;
    union {
      uint8_t   bytes[8];
      uint32_t  words[2];
      uint64_t  full_word;
    };
};

class CanRx {
  Can& parent;
public:
  CanRx(Can& can, struct capability::Token1<Can>) : parent(can){};
  virtual expected::expected<CanMessage, Error> read();
  virtual expected::expected<std::optional<CanMessage>, Error> try_read();
};

class CanTx {
  Can& parent;
public:
  CanTx(Can& can, struct capability::Token1<Can>) : parent(can){};
  virtual expected::expected<void, Error> write(const CanMessage& msg);
  virtual expected::expected<void, Error> try_write(const CanMessage& msg);
};

class CanInstanceSpecific;

class Can: public Driver {
  CanInstanceSpecific* p_instance_specific;

public:

  friend CanTx;
  friend CanRx;
  virtual CanTx& into_tx()&;
  virtual CanRx& into_rx()&;

  Can();
  static expected::expected<void, Error> start();
  expected::expected<void, Error> init(const Config&);
  expected::expected<void, Error> stop();

  virtual expected::expected<CanMessage, Error> read();
  virtual expected::expected<std::optional<CanMessage>, Error> try_read();
  virtual expected::expected<void, Error> write(const CanMessage& msg);
  virtual expected::expected<void, Error> try_write(const CanMessage& msg);
};

} // namespace ru::driver
