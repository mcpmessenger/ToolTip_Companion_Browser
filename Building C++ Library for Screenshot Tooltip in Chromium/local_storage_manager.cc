// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/tooltip/local_storage_manager.h"

#include "base/logging.h"

namespace tooltip {

LocalStorageManager::LocalStorageManager() = default;

LocalStorageManager::~LocalStorageManager() = default;

void LocalStorageManager::Initialize() {
  // No specific initialization needed for now. In a real Chromium
  // implementation, this might involve setting up a persistent storage
  // mechanism like LevelDB or PrefService.
}

void LocalStorageManager::StoreImage(
    const std::string& element_identifier, const std::string& base64_image) {
  image_cache_[element_identifier] = base64_image;
  VLOG(1) << "Stored image for element: " << element_identifier;
}

std::string LocalStorageManager::RetrieveImage(
    const std::string& element_identifier) {
  auto it = image_cache_.find(element_identifier);
  if (it != image_cache_.end()) {
    VLOG(1) << "Retrieved image for element: " << element_identifier;
    return it->second;
  }
  VLOG(1) << "Image not found for element: " << element_identifier;
  return std::string();
}

void LocalStorageManager::ClearStorage() {
  image_cache_.clear();
  VLOG(1) << "Cleared all stored images.";
}

}  // namespace tooltip


