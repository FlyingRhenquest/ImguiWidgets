/**
 * Copyright 2025 Bruce Ide
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.

 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <fr/RequirementsManager.h>
#include <fr/RequirementsManager/TaskNode.h>
#include <fr/RequirementsManager/ThreadPool.h>
#include <fr/Imgui/NodeEditorWindow.h>
#include <fr/Imgui/Registration.h>
#include <fr/ImguiWidgets.h>
#include <fr/types/Concepts.h>
#include <memory>
#include <unordered_map>

#ifndef NO_SQL
#include <fr/RequirementsManager/PqNodeFactory.h>
#endif

namespace fr::Imgui {

  template <typename WindowList>
  requires fr::types::IsUnique<WindowList>
  class NodeEditorWindow;

  template <typename WindowList>
  requires fr::types::IsUnique<WindowList>
  class WindowFactory;
  
  /**
   * A cleanup task I can just enqueue to the threadpool
   *
   * WindowFactory should be persistent for the lifetime of the
   * application, so this will just take a windowfactory raw
   * pointer.
   */

  template <typename WindowList, typename WorkerThreadType>
  class CleanupFactory : public fr::RequirementsManager::TaskNode<WorkerThreadType> {
    WindowFactory<WindowList> *_factory;
    std::string _uuid;
  public:
    using Type = CleanupFactory;
    using Parent = fr::RequirementsManager::TaskNode<WorkerThreadType>;
    using PtrType = std::shared_ptr<CleanupFactory<WindowList, WorkerThreadType>>;
    CleanupFactory(WindowFactory<WindowList> *factory, const std::string& uuid) : _factory(factory), _uuid(uuid) {
    }

    virtual ~CleanupFactory() {}

    void run() override {
      _factory->erase(_uuid);
    }
    
  };
  
  /**
   * WindowFactory takes a graph, creates properly-connected
   * windows for that graph and adds them to the NodeEditorWindow.
   */

  template <typename WindowList>
  requires fr::types::IsUnique<WindowList>
  class WindowFactory {
    // Map of node IDs in the graph
    std::unordered_map<std::string, std::shared_ptr<fr::RequirementsManager::Node>> _nodeMap;
    NodeEditorWindow<WindowList> *_editorWindow;
    std::shared_ptr<fr::RequirementsManager::ThreadPool<fr::RequirementsManager::WorkerThread>> _threadpool;
#ifndef NO_SQL
    std::unordered_map<std::string, std::shared_ptr<fr::RequirementsManager::PqNodeFactory<fr::RequirementsManager::WorkerThread>>> _factories;
#endif
    // Track IDs we add so we can connect them later
    std::mutex _addedIdsMutex;
    std::vector<std::string> _addedIds;

    // Handles establishing window connections. This gets called when all the windows
    // have loaded in
    void connect() {
      for (auto id : _addedIds) {
        auto window = _editorWindow->get(id);
        std::cout << "Connectiong " << id << std::endl;
        if (!window) {
          std::cout << "Error: Was not able to retrieve a window for " << id << std::endl;
          continue;
        }
        auto isCommitable = std::dynamic_pointer_cast<CommitableNodeWindow>(window);
        if (isCommitable) {
          // Check parent/children and see if we've created them yet
          auto node = std::dynamic_pointer_cast<fr::RequirementsManager::CommitableNode>(isCommitable->getNode());
          if (node) {
            // This should never NOT be a commitable node. Maybe I should throw if it isn't?
            // I only need to establish local relationships, I don't need to traverse the whole graph
            auto parent = node->getChangeParent();
            auto child = node->getChangeChild();
            if (parent) {
              auto changeParentWindow = dynamic_pointer_cast<CommitableNodeWindow>(_editorWindow->get(parent->idString()));
              if (changeParentWindow) {
                // Create a drag payload to connect the windows
                auto payload = std::make_shared<NodeDragPayload>();
                payload->dragSource = isCommitable->_leftAnchor;
                payload->sourceNode = node;
                payload->anchorType = AnchorType::Left;
                changeParentWindow->_rightAnchor->establishConnection(payload, false);
              }
            }
            if (child) {
              auto changeChildWindow = dynamic_pointer_cast<CommitableNodeWindow>(_editorWindow->get(child->idString()));
              if (changeChildWindow) {
                auto payload = std::make_shared<NodeDragPayload>();
                payload->dragSource = isCommitable->_rightAnchor;
                payload->sourceNode = node;
                payload->anchorType = AnchorType::Right;
                changeChildWindow->_leftAnchor->establishConnection(payload, false);
              }
            }
          }
        }

        auto isNode = std::dynamic_pointer_cast<NodeWindow>(window);
        if (isNode) {
          auto node = isNode->getNode();
          if (node) {
            // TODO: Should I throw if this is ever false? It should never happen.
            // Check node up and down lists
            for (auto upNode : node->up) {
              auto upNodeWindow = dynamic_pointer_cast<NodeWindow>(_editorWindow->get(upNode->idString()));
              if (upNodeWindow) {
                auto payload = std::make_shared<NodeDragPayload>();
                payload->dragSource = isNode->_upAnchor;
                payload->sourceNode = node;
                payload->anchorType = AnchorType::Up;
                upNodeWindow->_downAnchor->establishConnection(payload, false);
              }
            }
            for (auto downNode : node->down) {
              auto downNodeWindow = dynamic_pointer_cast<NodeWindow>(_editorWindow->get(downNode->idString()));
              if (downNodeWindow) {
                auto payload = std::make_shared<NodeDragPayload>();
                payload->dragSource = isNode->_downAnchor;
                payload->sourceNode = node;
                payload->anchorType = AnchorType::Down;
                downNodeWindow->_upAnchor->establishConnection(payload, false);
              }
            }
          }
        }
      }
    }
    
    void add(const std::string& id , Window::PtrType window) {
      if (_editorWindow) {
        std::lock_guard<std::mutex> lock(_addedIdsMutex);
        _editorWindow->add(id, window);
        _addedIds.push_back(id);
       }
    }    
    
    template <typename Windows>
    requires fr::types::IsUnique<Windows>
    void createWindow(std::shared_ptr<fr::RequirementsManager::Node> node) {
      // Find node type
      using CurrentWindowNodeType = Windows::head::type;
      if constexpr (!std::is_void_v<CurrentWindowNodeType>) {
        // Try casting node to current node type
        using NodeType = Registration::Record<CurrentWindowNodeType>::NodeType;
        auto n = std::dynamic_pointer_cast<NodeType>(node);
        if (n) {
          // This is the correct node type
          using WindowNodeType = Registration::Record<CurrentWindowNodeType>::Type;
          auto window = std::make_shared<WindowNodeType>();
          window->setStartingSize(Registration::Record<WindowNodeType>::startingSize());
          window->addNode(node);
          window->init();
          add(node->idString(), window);
        } else {
          if constexpr (!std::is_void_v<typename Windows::tail::head::type>) {
            this->createWindow<typename Windows::tail>(node);
          }
        }
      }
    }
    
  public:

    WindowFactory() : _editorWindow(nullptr) {
      _threadpool = std::make_shared<fr::RequirementsManager::ThreadPool<fr::RequirementsManager::WorkerThread>>();
      _threadpool->startThreads(4);
    }
    
    ~WindowFactory() {
      _threadpool->shutdown();
      _threadpool->join();
    }

    // Add an editor window so we can create windows with it
    void addEditorWindow(NodeEditorWindow<WindowList> *editor) {
      _editorWindow = editor;
    }

    // Add a graph to the factory. WindowList is a list of supported windows.
    // This function will read the registration records and try to find the
    // correct window to create based on the NodeType in the registration
    // record.
    void add(std::shared_ptr<fr::RequirementsManager::Node> node) {
      node->traverse([&](fr::RequirementsManager::Node::PtrType node) {
        this->createWindow<WindowList>(node);
      });
      connect();
    }

    // Remove a factory by UUID (for factory cleanup)
#ifndef NO_SQL
    void erase(const std::string& uuid) {
      _factories.erase(uuid);
    }

    void load(const std::string& uuid) {
      if (!_factories.contains(uuid)) {
        _factories[uuid] = std::make_shared<fr::RequirementsManager::PqNodeFactory<fr::RequirementsManager::WorkerThread>>(uuid);
        _factories[uuid]->loaded.connect([&](const std::string& uuid, fr::RequirementsManager::Node::PtrType node){
          this->add(node);
        });
        _factories[uuid]->done.connect([&](const std::string& uuid){
          connect();
          // Enqueue a cleaner to run once this signal handler returns
          //  auto cleaner = std::make_shared<CleanupFactory<WindowList,fr::RequirementsManager::WorkerThread>>(this, uuid);
          // _threadpool->enqueue(cleaner);
        });
        _threadpool->enqueue(_factories[uuid]);
      }
    }
#endif
  };
  
}
