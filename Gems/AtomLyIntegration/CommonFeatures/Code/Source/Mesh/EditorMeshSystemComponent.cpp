/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/EditContextConstants.inl>
#include <AzCore/Utils/Utils.h>
#include <AzToolsFramework/Thumbnails/ThumbnailContext.h>
#include <Source/Mesh/EditorMeshSystemComponent.h>
#include <Source/Mesh/MeshThumbnail.h>

namespace AZ
{
    namespace Render
    {
        //! Main system component for the Atom Common Feature Gem's editor/tools module.
        void EditorMeshSystemComponent::Reflect(AZ::ReflectContext* context)
        {
            if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
            {
                serialize->Class<EditorMeshSystemComponent, AZ::Component>()
                    ->Version(0);

                if (AZ::EditContext* ec = serialize->GetEditContext())
                {
                    ec->Class<EditorMeshSystemComponent>("EditorMeshSystemComponent", "System component that sets up necessary logic related to EditorMeshComponent..")
                        ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("System"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                        ;
                }
            }
        }

        void EditorMeshSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC_CE("EditorMeshSystem"));
        }

        void EditorMeshSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC_CE("EditorMeshSystem"));
        }

        void EditorMeshSystemComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {
            required.push_back(AZ_CRC_CE("ThumbnailerService"));
        }

        void EditorMeshSystemComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
        {
            AZ_UNUSED(dependent);
        }
        
        void EditorMeshSystemComponent::Activate()
        {
            AzFramework::ApplicationLifecycleEvents::Bus::Handler::BusConnect();
            SetupThumbnails();
        }

        void EditorMeshSystemComponent::Deactivate()
        {
            TeardownThumbnails();
            AzFramework::ApplicationLifecycleEvents::Bus::Handler::BusDisconnect();
        }

        void EditorMeshSystemComponent::OnApplicationAboutToStop()
        {
            TeardownThumbnails();
        }

        void EditorMeshSystemComponent::SetupThumbnails()
        {
            using namespace AzToolsFramework::Thumbnailer;
            using namespace LyIntegration;
            
            ThumbnailerRequestsBus::Broadcast(&ThumbnailerRequests::RegisterThumbnailProvider,
                MAKE_TCACHE(Thumbnails::MeshThumbnailCache),
                ThumbnailContext::DefaultContext);
        }

        void EditorMeshSystemComponent::TeardownThumbnails()
        {
            using namespace AzToolsFramework::Thumbnailer;
            using namespace LyIntegration;

            ThumbnailerRequestsBus::Broadcast(&ThumbnailerRequests::UnregisterThumbnailProvider,
                Thumbnails::MeshThumbnailCache::ProviderName,
                ThumbnailContext::DefaultContext);
        }
    } // namespace Render
} // namespace AZ
