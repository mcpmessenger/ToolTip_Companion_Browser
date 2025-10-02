// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_TOOLTIP_LOCAL_STORAGE_MANAGER_H_
#define CHROME_BROWSER_TOOLTIP_LOCAL_STORAGE_MANAGER_H_

#include <map>
#include <string>

#include "base/memory/weak_ptr.h"

namespace tooltip {

// Manages client-side storage of Base64 encoded images.
class LocalStorageManager {
 public:
  LocalStorageManager();
  ~LocalStorageManager();

  // Initializes the storage system.
  void Initialize();

  // Stores a Base64 encoded image associated with an element identifier.
  void StoreImage(const std::string& element_identifier, const std::string& base64_image);

  // Retrieves a Base64 encoded image for a given element identifier.
  // Returns an empty string if not found.
  std::string RetrieveImage(const std::string& element_identifier);

  // Clears all stored images.
  void ClearStorage();

 private:
  // In-memory cache for stored images.
  std::map<std::string, std::string> image_cache_;

  base::WeakPtrFactory<LocalStorageManager> weak_ptr_factory_{this};
};

}  // namespace tooltip

#endif  // CHROME_BROWSER_TOOLTIP_LOCAL_STORAGE_MANAGER_H_

