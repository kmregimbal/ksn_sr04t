#include "ksn_sr04t.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

// Very basic support for JSN_SR04T V3.0 distance sensor in mode 2

namespace esphome {
namespace ksn_sr04t {

static const char *const TAG = "ksn_sr04t.sensor";

void Ksnsr04tComponent::update() {
  this->write_byte(0x55);
  ESP_LOGV(TAG, "Request read out from sensor");
}

void Ksnsr04tComponent::loop() {
  while (this->available() > 0) {
    uint8_t data;
    this->read_byte(&data);

    ESP_LOGV(TAG, "Read byte from sensor: %x", data);

    if (this->buffer_.empty() && data != 0xFF)
      continue;

    this->buffer_.push_back(data);
    if (this->buffer_.size() == 4)
      this->check_buffer_();
  }
}

void Ksnsr04tComponent::check_buffer_() {
  uint8_t checksum = 0;
  switch (this->model_) {
    case KSN_SR04T:
      checksum = this->buffer_[0] + this->buffer_[1] + this->buffer_[2];
      break;
    case AJ_SR04M:
      checksum = this->buffer_[1] + this->buffer_[2];
      break;
    case AJ_SR04M2:
      checksum = (this->buffer_[0] + this->buffer_[1] + this->buffer_[2]) & 0x00ff;
  }

  if (this->buffer_[3] == checksum) {
    uint16_t distance = encode_uint16(this->buffer_[1], this->buffer_[2]);
    if (distance >= 190) {
      float centimeters = distance / 10.0f;
      ESP_LOGV(TAG, "Distance from sensor: %umm, %.3fm", distance, meters);
      this->publish_state(centimeters);
    } else {
      ESP_LOGW(TAG, "Invalid distance (%i) data read from sensor: %s", distance, format_hex_pretty(this->buffer_).c_str());
    }
  } else {
    ESP_LOGW(TAG, "checksum failed: %02x != %02x", checksum, this->buffer_[3]);
  }
  this->buffer_.clear();
}

void Ksnsr04tComponent::dump_config() {
  LOG_SENSOR("", "JST_SR04T Sensor", this);
  switch (this->model_) {
    case KSN_SR04T:
      ESP_LOGCONFIG(TAG, "  sensor model: ksn_sr04t");
      break;
    case AJ_SR04M:
      ESP_LOGCONFIG(TAG, "  sensor model: aj_sr04m");
      break;
    case AJ_SR04M2:
      ESP_LOGCONFIG(TAG, "  sensor model: aj_sr04m2");
      break;
  }
  LOG_UPDATE_INTERVAL(this);
}

}  // namespace ksn_sr04t
}  // namespace esphome