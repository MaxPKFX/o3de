/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */
#pragma once

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/UserSettings/UserSettings.h>
#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/std/containers/unordered_map.h>
#include <AzCore/Serialization/SerializeContext.h>

namespace AZ { namespace Data { class AssetData; } }

namespace AZStd
{
    template<>
    struct hash<AZ::Data::Asset<AZ::Data::AssetData>>
    {
        size_t operator()(const AZ::Data::Asset<AZ::Data::AssetData>& asset) const
        {
            return asset.GetId().m_guid.GetHash();
        }
    };
}

namespace AzToolsFramework
{
    class InstanceDataNode;

    namespace AssetEditor
    {
        // This class is used to track all of the asset editors that were open.  When the editor launches
        // these settings will be used to preemptively register all windows and then open them.
        struct AssetEditorWindowSettings : AZ::UserSettings
        {
            AZ_CLASS_ALLOCATOR(AssetEditorWindowSettings, AZ::SystemAllocator, 0);
            AZ_RTTI(AssetEditorWindowSettings, "{981FE4FF-0B56-4115-9F75-79609E3D6337}", AZ::UserSettings);

            //! The list of opened assets is used to restore their window state
            AZStd::unordered_set<AZ::Data::Asset<AZ::Data::AssetData>> m_openAssets;

            static constexpr const char* s_name = "AssetEditorWindowSettings";

            static void Reflect(AZ::ReflectContext* context)
            {
                if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
                {
                    serializeContext->Class<AssetEditorWindowSettings>()
                        ->Field("m_openAssets", &AssetEditorWindowSettings::m_openAssets)
                        ;
                }
            }
        };

        // External interaction with Asset Editor
        class AssetEditorRequests
            : public AZ::EBusTraits
        {
        public:
            static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;

            //! Open Asset Editor (if it is not already open) and create a new asset of type specified, if possible
            virtual void CreateNewAsset(const AZ::Data::AssetType& assetType) = 0;

            virtual AZ::Outcome<bool, AZStd::string> IsAssetDataValid() { return AZ::Success(true); }

            //! Open Asset Editor (if it is not already open) and load asset by reference
            virtual void OpenAssetEditor(const AZ::Data::Asset<AZ::Data::AssetData>& asset) = 0;
            //! Open Asset Editor (if it is not already open) and load asset by id
            virtual void OpenAssetEditorById(const AZ::Data::AssetId assetId) = 0;
        };
        using AssetEditorRequestsBus = AZ::EBus<AssetEditorRequests>;

        class AssetEditorValidationRequests
            : public AZ::EBusTraits
        {
        public:

            using BusIdType = AZ::Data::AssetId;
            static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Multiple;
            static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::ById;

            virtual void BeforePropertyEdit(AzToolsFramework::InstanceDataNode* node, AZ::Data::Asset<AZ::Data::AssetData> asset) { AZ_UNUSED(node); AZ_UNUSED(asset); }

            virtual void PreAssetSave(AZ::Data::Asset<AZ::Data::AssetData> asset) { AZ_UNUSED(asset); }

            virtual AZ::Outcome<bool, AZStd::string> IsAssetDataValid(const AZ::Data::Asset<AZ::Data::AssetData>& asset) { AZ_UNUSED(asset); return AZ::Success(true); }
        };
        using AssetEditorValidationRequestBus = AZ::EBus<AssetEditorValidationRequests>;

        // Internal interaction with existing Asset Editor widget
        class AssetEditorWidgetRequests
            : public AZ::EBusTraits
        {
        public:
            static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;

            //! Creates a new asset of type provided (if possible) in the currently open Asset Editor window
            virtual void CreateAsset(const AZ::Data::AssetType& assetType) = 0;
            //! Saves the asset being edited in the currently open Asset Editor window to the path provided
            virtual void SaveAssetAs(const AZStd::string_view assetPath) = 0;
            //! Opens the asset provided (by reference) in the currently open Asset Editor window
            virtual void OpenAsset(const AZ::Data::Asset<AZ::Data::AssetData>& asset) = 0;
            //! Opens the asset provided (by id) in the currently open Asset Editor window
            virtual void OpenAssetById(const AZ::Data::AssetId assetId) = 0;
        };
        using AssetEditorWidgetRequestsBus = AZ::EBus<AssetEditorWidgetRequests>;
    } // namespace AssetEditor
} // namespace AzToolsFramework
