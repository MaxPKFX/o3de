/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <AzTest/AzTest.h>
#include <Common/RPITestFixture.h>
#include <Common/JsonTestUtils.h>
#include <Atom/RPI.Edit/Material/MaterialTypeSourceData.h>
#include <Atom/RPI.Edit/Material/MaterialPropertySerializer.h>
#include <AtomCore/Serialization/Json/JsonUtils.h>
#include <Common/TestUtils.h>
#include <Tests/Serialization/Json/JsonSerializerConformityTests.h>

namespace JsonSerializationTests
{
    class MaterialPropertySerializerTestDescription :
        public JsonSerializerConformityTestDescriptor<AZ::RPI::MaterialTypeSourceData::PropertyDefinition>
    {
    public:
        void Reflect(AZStd::unique_ptr<AZ::SerializeContext>& context)
        {
            AZ::RPI::MaterialTypeSourceData::Reflect(context.get());
            AZ::RPI::MaterialPropertyDescriptor::Reflect(context.get());
            AZ::RPI::ReflectMaterialDynamicMetadata(context.get());
        }

        void Reflect(AZStd::unique_ptr<AZ::JsonRegistrationContext>& context)
        {
            AZ::RPI::MaterialTypeSourceData::Reflect(context.get());
        }

        AZStd::shared_ptr<AZ::BaseJsonSerializer> CreateSerializer() override
        {
            return AZStd::make_shared<AZ::RPI::JsonMaterialPropertySerializer>();
        }

        AZStd::shared_ptr<AZ::RPI::MaterialTypeSourceData::PropertyDefinition> CreateDefaultInstance() override
        {
            return AZStd::make_shared<AZ::RPI::MaterialTypeSourceData::PropertyDefinition>();
        }

        AZStd::shared_ptr<AZ::RPI::MaterialTypeSourceData::PropertyDefinition> CreatePartialDefaultInstance() override
        {
            auto result = AZStd::make_shared<AZ::RPI::MaterialTypeSourceData::PropertyDefinition>();
            result->m_nameId = "testProperty";
            result->m_dataType = AZ::RPI::MaterialPropertyDataType::Float;
            result->m_step = 1.0f;
            result->m_value = 0.0f;
            return result;
        }

        AZStd::string_view GetJsonForPartialDefaultInstance() override
        {
            return R"(
            {
                "id": "testProperty",
                "type": "Float",
                "step": 1.0
            })";
        }

        AZStd::shared_ptr<AZ::RPI::MaterialTypeSourceData::PropertyDefinition> CreateFullySetInstance() override
        {
            auto result = AZStd::make_shared<AZ::RPI::MaterialTypeSourceData::PropertyDefinition>();
            result->m_nameId = "testProperty";
            result->m_description = "description";
            result->m_displayName = "display_name";
            result->m_dataType = AZ::RPI::MaterialPropertyDataType::Float;
            result->m_value = 2.0f;
            result->m_enumIsUv = true;
            result->m_min = 1.0f;
            result->m_max = 10.0f;
            result->m_softMin = 2.0f;
            result->m_softMax = 9.0f;
            result->m_step = 1.5f;
            result->m_visibility = AZ::RPI::MaterialPropertyVisibility::Hidden;
            result->m_outputConnections.emplace_back(AZ::RPI::MaterialPropertyOutputType::ShaderOption, "o_foo", 2);

            return result;
        }

        AZStd::string_view GetJsonForFullySetInstance() override
        {
            return R"(
            {
                "id": "testProperty",
                "displayName": "display_name",
                "description": "description",
                "type": "Float",
                "defaultValue": 2.0,
                "min": 1.0,
                "max": 10.0,
                "softMin": 2.0,
                "softMax": 9.0,
                "step": 1.5,
                "visibility": "Hidden",
                "connection":
                {
                    "type": "ShaderOption",
                    "id": "o_foo",
                    "shaderIndex": 2
                },
                "enumIsUv": true
            })";
        }

        void ConfigureFeatures(JsonSerializerConformityTestDescriptorFeatures& features) override
        {
            features.EnableJsonType(rapidjson::kObjectType);
        }

        bool CompareFloatAny(const AZStd::any& lhs, const AZStd::any& rhs)
        {
            if (!lhs.is<float>() || !rhs.is<float>())
            {
                return lhs.empty() && rhs.empty();
            }

            const float* lhsValue = AZStd::any_cast<float>(&lhs);
            const float* rhsValue = AZStd::any_cast<float>(&rhs);

            if (lhsValue == nullptr || rhsValue == nullptr)
            {
                return false;
            }

            return *lhsValue == *rhsValue;
        }

        bool AreEqual(
            const AZ::RPI::MaterialTypeSourceData::PropertyDefinition& lhs,
            const AZ::RPI::MaterialTypeSourceData::PropertyDefinition& rhs) override
        {
            if (lhs.m_nameId != rhs.m_nameId) { return false; }
            if (lhs.m_description != rhs.m_description) { return false; }
            if (lhs.m_displayName != rhs.m_displayName) { return false; }
            if (lhs.m_dataType != rhs.m_dataType) { return false; }
            if (lhs.m_value != rhs.m_value) { return false; }
            if (lhs.m_enumIsUv != rhs.m_enumIsUv) { return false; }
            if (lhs.m_min != rhs.m_min) { return false; }
            if (lhs.m_max != rhs.m_max) { return false; }
            if (lhs.m_softMin != rhs.m_softMin) { return false; }
            if (lhs.m_softMax != rhs.m_softMax) { return false; }
            if (lhs.m_step != rhs.m_step) { return false; }
            if (lhs.m_visibility != rhs.m_visibility) { return false; }
            if (lhs.m_outputConnections.size() != rhs.m_outputConnections.size()) { return false; }
            for (size_t i = 0; i < lhs.m_outputConnections.size(); ++i)
            {
                auto& leftConnection = lhs.m_outputConnections[i];
                auto& rightConnection = rhs.m_outputConnections[i];
                if (leftConnection.m_type != rightConnection.m_type) { return false; }
                if (leftConnection.m_nameId != rightConnection.m_nameId) { return false; }
                if (leftConnection.m_shaderIndex != rightConnection.m_shaderIndex) { return false; }
            }
            return true;
        }
    };

    using MaterialPropertySerializerTestTypes = ::testing::Types<MaterialPropertySerializerTestDescription>;
    INSTANTIATE_TYPED_TEST_CASE_P(MaterialPropertySerializerTests, JsonSerializerConformityTests, MaterialPropertySerializerTestTypes);
} // namespace JsonSerializationTests

namespace UnitTest
{
    using namespace AZ;
    using namespace RPI;

    class MaterialPropertySerializerTests
        : public RPITestFixture
    {
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Reflect(ReflectContext* context) override
        {
            RPITestFixture::Reflect(context);
            MaterialTypeSourceData::Reflect(context);
        }
        
        template<typename T>
        void TestStoreToJson(const T& object, AZStd::string_view expectedJson)
        {
            AZStd::string outputJson;
            JsonTestResult storeResult = StoreTestDataToJson(object, outputJson);

            EXPECT_EQ(AZ::JsonSerializationResult::Tasks::WriteValue, storeResult.m_jsonResultCode.GetTask());
            EXPECT_EQ(AZ::JsonSerializationResult::Processing::Completed, storeResult.m_jsonResultCode.GetProcessing());
            EXPECT_EQ(AZ::JsonSerializationResult::Outcomes::PartialDefaults, storeResult.m_jsonResultCode.GetOutcome());

            ExpectSimilarJson(expectedJson, outputJson);
        }
    };


    // "GeneraData" refers to data that isn't dependent on the "type" field
    TEST_F(MaterialPropertySerializerTests, LoadAndStoreJson_GeneralData)
    {
        const AZStd::string inputJson = R"(
        {
            "id": "testProperty",
            "displayName": "Test Property",
            "description": "This is a property description",
            "type": "Float"
        }
        )";

        MaterialTypeSourceData::PropertyDefinition propertyData;
        JsonTestResult loadResult = LoadTestDataFromJson(propertyData, inputJson);

        EXPECT_EQ(AZ::JsonSerializationResult::Tasks::ReadField, loadResult.m_jsonResultCode.GetTask());
        EXPECT_EQ(AZ::JsonSerializationResult::Processing::Completed, loadResult.m_jsonResultCode.GetProcessing());
        EXPECT_EQ(AZ::JsonSerializationResult::Outcomes::PartialDefaults, loadResult.m_jsonResultCode.GetOutcome());

        EXPECT_EQ("testProperty", propertyData.m_nameId);
        EXPECT_EQ("Test Property", propertyData.m_displayName);
        EXPECT_EQ("This is a property description", propertyData.m_description);
        EXPECT_EQ(MaterialPropertyDataType::Float, propertyData.m_dataType);

        EXPECT_TRUE(loadResult.ContainsMessage("/id", "Success"));
        EXPECT_TRUE(loadResult.ContainsMessage("/displayName", "Success"));
        EXPECT_TRUE(loadResult.ContainsMessage("/description", "Success"));
        EXPECT_TRUE(loadResult.ContainsMessage("/type", "Success"));

        EXPECT_FALSE(loadResult.ContainsOutcome(JsonSerializationResult::Outcomes::Skipped));

        TestStoreToJson(propertyData, inputJson);
    }

    // "GeneraData" refers to data that isn't dependent on the "type" field
    TEST_F(MaterialPropertySerializerTests, LoadAndStoreJson_DefaultGeneralData)
    {
        // Note we are keeping id and type because they are required fields
        const AZStd::string inputJson = R"(
        {
            "id": "testProperty",
            "type": "Float"
        }
        )";

        MaterialTypeSourceData::PropertyDefinition propertyData;
        JsonTestResult loadResult = LoadTestDataFromJson(propertyData, inputJson);

        EXPECT_EQ(AZ::JsonSerializationResult::Tasks::ReadField, loadResult.m_jsonResultCode.GetTask());
        EXPECT_EQ(AZ::JsonSerializationResult::Processing::Completed, loadResult.m_jsonResultCode.GetProcessing());
        EXPECT_EQ(AZ::JsonSerializationResult::Outcomes::PartialDefaults, loadResult.m_jsonResultCode.GetOutcome());

        EXPECT_TRUE(propertyData.m_displayName.empty());
        EXPECT_TRUE(propertyData.m_description.empty());

        EXPECT_TRUE(loadResult.ContainsMessage("/id", "Success"));
        EXPECT_TRUE(loadResult.ContainsMessage("/type", "Success"));

        EXPECT_FALSE(loadResult.ContainsOutcome(JsonSerializationResult::Outcomes::Skipped));

        TestStoreToJson(propertyData, inputJson);
    }

    TEST_F(MaterialPropertySerializerTests, Load_Error_NotAnObject)
    {
        const AZStd::string inputJson = R"(
        []
        )";

        MaterialTypeSourceData::PropertyDefinition propertyData;
        JsonTestResult loadResult = LoadTestDataFromJson(propertyData, inputJson);

        EXPECT_EQ(AZ::JsonSerializationResult::Tasks::ReadField, loadResult.m_jsonResultCode.GetTask());
        EXPECT_EQ(AZ::JsonSerializationResult::Processing::Altered, loadResult.m_jsonResultCode.GetProcessing());
        EXPECT_EQ(AZ::JsonSerializationResult::Outcomes::Unsupported, loadResult.m_jsonResultCode.GetOutcome());

        EXPECT_TRUE(loadResult.ContainsMessage("", "Property definition must be a JSON object"));
    }
    
    TEST_F(MaterialPropertySerializerTests, Load_Error_InvalidDataType)
    {
        const AZStd::string inputJson = R"(
        {
            "id": "testProperty",
            "type": "foo"
        }
        )";

        MaterialTypeSourceData::PropertyDefinition propertyData;
        JsonTestResult loadResult = LoadTestDataFromJson(propertyData, inputJson);
        
        EXPECT_EQ(AZ::JsonSerializationResult::Tasks::ReadField, loadResult.m_jsonResultCode.GetTask());
        EXPECT_EQ(AZ::JsonSerializationResult::Processing::PartialAlter, loadResult.m_jsonResultCode.GetProcessing());
        EXPECT_EQ(AZ::JsonSerializationResult::Outcomes::Unsupported, loadResult.m_jsonResultCode.GetOutcome());

        EXPECT_EQ(AZ::RPI::MaterialPropertyDataType::Invalid, propertyData.m_dataType);

        EXPECT_TRUE(loadResult.ContainsMessage("/id", "Success"));
        EXPECT_TRUE(loadResult.ContainsMessage("/type", "Enum value could not read"));
    }

    TEST_F(MaterialPropertySerializerTests, LoadAndStoreJson_NumericType_AllValues)
    {
        const AZStd::string inputJson = R"(
        [
            {
                "id": "testProperty1",
                "type": "Float",
                "defaultValue": 0.5,
                "min": 0.1,
                "max": 1.5,
                "softMin": 0.2,
                "softMax": 1.0,
                "step": 0.05
            },
            {
                "id": "testProperty2",
                "type": "Int",
                "defaultValue": -1,
                "min": -5,
                "max": 5,
                "softMin": -4,
                "softMax": 4,
                "step": 1
            },
            {
                "id": "testProperty3",
                "type": "UInt",
                "defaultValue": 4294901761,
                "min": 4294901760,
                "max": 4294901775,
                "softMin": 4294901761,
                "softMax": 4294901774,
                "step": 1
            }
        ]
        )";

        AZStd::vector<MaterialTypeSourceData::PropertyDefinition> propertyData;
        JsonTestResult loadResult = LoadTestDataFromJson(propertyData, inputJson);

        EXPECT_EQ(AZ::JsonSerializationResult::Tasks::ReadField, loadResult.m_jsonResultCode.GetTask());
        EXPECT_EQ(AZ::JsonSerializationResult::Processing::Completed, loadResult.m_jsonResultCode.GetProcessing());

        EXPECT_EQ(MaterialPropertyDataType::Float, propertyData[0].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(0.5f), propertyData[0].m_value);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(0.1f), propertyData[0].m_min);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(1.5f), propertyData[0].m_max);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(0.2f), propertyData[0].m_softMin);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(1.0f), propertyData[0].m_softMax);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(0.05f), propertyData[0].m_step);

        EXPECT_EQ(MaterialPropertyDataType::Int, propertyData[1].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(-1), propertyData[1].m_value);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(-5), propertyData[1].m_min);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(5), propertyData[1].m_max);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(-4), propertyData[1].m_softMin);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(4), propertyData[1].m_softMax);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(1), propertyData[1].m_step);

        EXPECT_EQ(MaterialPropertyDataType::UInt, propertyData[2].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(0xFFFF0001u), propertyData[2].m_value);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(0xFFFF0000u), propertyData[2].m_min);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(0xFFFF000Fu), propertyData[2].m_max);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(0xFFFF0001u), propertyData[2].m_softMin);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(0xFFFF000Eu), propertyData[2].m_softMax);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(1u), propertyData[2].m_step);

        for (int i = 0; i < propertyData.size(); ++i)
        {
            AZStd::string prefix = AZStd::string::format("/%d", i);
            EXPECT_TRUE(loadResult.ContainsMessage(prefix + "/id", "Success"));
            EXPECT_TRUE(loadResult.ContainsMessage(prefix + "/type", "Success"));
            EXPECT_TRUE(loadResult.ContainsMessage(prefix + "/defaultValue", "Success"));
            EXPECT_TRUE(loadResult.ContainsMessage(prefix + "/min", "Success"));
            EXPECT_TRUE(loadResult.ContainsMessage(prefix + "/max", "Success"));
            EXPECT_TRUE(loadResult.ContainsMessage(prefix + "/softMin", "Success"));
            EXPECT_TRUE(loadResult.ContainsMessage(prefix + "/softMax", "Success"));
            EXPECT_TRUE(loadResult.ContainsMessage(prefix + "/step", "Success"));
        }

        EXPECT_FALSE(loadResult.ContainsOutcome(JsonSerializationResult::Outcomes::Skipped));

        TestStoreToJson(propertyData, inputJson);
    }

    TEST_F(MaterialPropertySerializerTests, LoadAndStoreJson_NumericType_DefaultValues)
    {
        const AZStd::string inputJson = R"(
        [
            {
                "id": "testProperty1",
                "displayName": "Test Property 1",
                "description": "Test",
                "type": "Float"
            },
            {
                "id": "testProperty2",
                "displayName": "Test Property 2",
                "description": "Test",
                "type": "Int"
            },
            {
                "id": "testProperty3",
                "displayName": "Test Property 3",
                "description": "Test",
                "type": "UInt"
            }
        ]
        )";

        AZStd::vector<MaterialTypeSourceData::PropertyDefinition> propertyData;
        JsonTestResult loadResult = LoadTestDataFromJson(propertyData, inputJson);

        EXPECT_EQ(AZ::JsonSerializationResult::Tasks::ReadField, loadResult.m_jsonResultCode.GetTask());
        EXPECT_EQ(AZ::JsonSerializationResult::Processing::Completed, loadResult.m_jsonResultCode.GetProcessing());
        EXPECT_EQ(AZ::JsonSerializationResult::Outcomes::PartialDefaults, loadResult.m_jsonResultCode.GetOutcome());

        EXPECT_EQ(MaterialPropertyDataType::Float, propertyData[0].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(0.0f), propertyData[0].m_value);

        EXPECT_EQ(MaterialPropertyDataType::Int, propertyData[1].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(0), propertyData[1].m_value);

        EXPECT_EQ(MaterialPropertyDataType::UInt, propertyData[2].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(0u), propertyData[2].m_value);

        for (const MaterialTypeSourceData::PropertyDefinition& property : propertyData)
        {
            EXPECT_FALSE(property.m_min.IsValid());
            EXPECT_FALSE(property.m_max.IsValid());
            EXPECT_FALSE(property.m_softMin.IsValid());
            EXPECT_FALSE(property.m_softMax.IsValid());
            EXPECT_FALSE(property.m_step.IsValid());
        }

        EXPECT_FALSE(loadResult.ContainsOutcome(JsonSerializationResult::Outcomes::Skipped));

        TestStoreToJson(propertyData, inputJson);
    }

    TEST_F(MaterialPropertySerializerTests, LoadAndStoreJson_VectorLabels_LabelValues)
    {
        const AZStd::string inputJson = R"(
        [
            {
                "id": "testProperty1",
                "type": "Vector2",
                "vectorLabels": ["U", "V"],
                "defaultValue": [0.6, 0.5]
            },
            {
                "id": "testProperty2",
                "type": "Vector4",
                "vectorLabels": ["A", "B", "C", "D"],
                "defaultValue": [0.3, 0.4, 0.5, 0.6]
            }
        ]
        )";

        AZStd::vector<MaterialTypeSourceData::PropertyDefinition> propertyData;
        JsonTestResult loadResult = LoadTestDataFromJson(propertyData, inputJson);

        EXPECT_EQ(AZ::JsonSerializationResult::Tasks::ReadField, loadResult.m_jsonResultCode.GetTask());
        EXPECT_EQ(AZ::JsonSerializationResult::Processing::Completed, loadResult.m_jsonResultCode.GetProcessing());

        EXPECT_EQ(MaterialPropertyDataType::Vector2, propertyData[0].m_dataType);
        EXPECT_TRUE(propertyData[0].m_vectorLabels.size() == 2);
        EXPECT_EQ("U", propertyData[0].m_vectorLabels[0]);
        EXPECT_EQ("V", propertyData[0].m_vectorLabels[1]);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(AZ::Vector2{ 0.6f, 0.5f }), propertyData[0].m_value);

        EXPECT_EQ(MaterialPropertyDataType::Vector4, propertyData[1].m_dataType);
        EXPECT_TRUE(propertyData[1].m_vectorLabels.size() == 4);
        EXPECT_EQ("A", propertyData[1].m_vectorLabels[0]);
        EXPECT_EQ("B", propertyData[1].m_vectorLabels[1]);
        EXPECT_EQ("C", propertyData[1].m_vectorLabels[2]);
        EXPECT_EQ("D", propertyData[1].m_vectorLabels[3]);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(AZ::Vector4{ 0.3f, 0.4f, 0.5f, 0.6f }), propertyData[1].m_value);

        TestStoreToJson(propertyData, inputJson);
    }

    TEST_F(MaterialPropertySerializerTests, LoadAndStoreJson_Visibility)
    {
        const AZStd::string inputJson = R"(
        [
            {
                "id": "visibilityIsDefault",
                "type": "Float"
            },
            {
                "id": "visibilityIsEditable",
                "type": "Float",
                "visibility": "Enabled"
            },
            {
                "id": "visibilityIsDisabled",
                "type": "Float",
                "visibility": "Disabled"
            },
            {
                "id": "visibilityIsHidden",
                "type": "Float",
                "visibility": "Hidden"
            }
        ]
        )";

        const AZStd::string expectedOutputJson = R"(
        [
            {
                "id": "visibilityIsDefault",
                "type": "Float"
            },
            {
                "id": "visibilityIsEditable",
                "type": "Float"
            },
            {
                "id": "visibilityIsDisabled",
                "type": "Float",
                "visibility": "Disabled"
            },
            {
                "id": "visibilityIsHidden",
                "type": "Float",
                "visibility": "Hidden"
            }
        ]
        )";

        AZStd::vector<MaterialTypeSourceData::PropertyDefinition> propertyData;
        JsonTestResult loadResult = LoadTestDataFromJson(propertyData, inputJson);

        EXPECT_EQ(AZ::JsonSerializationResult::Tasks::ReadField, loadResult.m_jsonResultCode.GetTask());
        EXPECT_EQ(AZ::JsonSerializationResult::Processing::Completed, loadResult.m_jsonResultCode.GetProcessing());
        EXPECT_EQ(AZ::JsonSerializationResult::Outcomes::PartialDefaults, loadResult.m_jsonResultCode.GetOutcome()); // Because other fields like description are not included

        EXPECT_EQ(propertyData[0].m_visibility, MaterialPropertyVisibility::Enabled);
        EXPECT_EQ(propertyData[1].m_visibility, MaterialPropertyVisibility::Enabled);
        EXPECT_EQ(propertyData[2].m_visibility, MaterialPropertyVisibility::Disabled);
        EXPECT_EQ(propertyData[3].m_visibility, MaterialPropertyVisibility::Hidden);

        TestStoreToJson(propertyData, expectedOutputJson);
    }

    TEST_F(MaterialPropertySerializerTests, Load_NumericType_AlternateValueRepresentation)
    {
        // These alternate representations are supported by the fact that default JSON serializers
        // for numeric values use a flexible "best-effort" paradigm

        const AZStd::string inputJson = R"(
        [
            {
                "id": "testProperty1",
                "type": "Float",
                "defaultValue": true,
                "min": -1,
                "max": "100.5",
                "step": "1"
            },
            {
                "id": "testProperty2",
                "type": "Int",
                "defaultValue": true,
                "min": -1.5,
                "max": "100",
                "step": "1"
            },
            {
                "id": "testProperty3",
                "type": "UInt",
                "defaultValue": "4294963200",
                "min": true,
                "max": "0xFFFFFF00",
                "step": 2.5
            }
        ]
        )";

        AZStd::vector<MaterialTypeSourceData::PropertyDefinition> propertyData;
        JsonTestResult loadResult = LoadTestDataFromJson(propertyData, inputJson);

        EXPECT_EQ(AZ::JsonSerializationResult::Tasks::ReadField, loadResult.m_jsonResultCode.GetTask());
        EXPECT_EQ(AZ::JsonSerializationResult::Processing::Completed, loadResult.m_jsonResultCode.GetProcessing());

        EXPECT_EQ(MaterialPropertyDataType::Float, propertyData[0].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(1.0f), propertyData[0].m_value);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(-1.0f), propertyData[0].m_min);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(100.5f), propertyData[0].m_max);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(1.0f), propertyData[0].m_step);

        EXPECT_EQ(MaterialPropertyDataType::Int, propertyData[1].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(1), propertyData[1].m_value);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(-1), propertyData[1].m_min);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(100), propertyData[1].m_max);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(1), propertyData[1].m_step);

        EXPECT_EQ(MaterialPropertyDataType::UInt, propertyData[2].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(0xFFFFF000u), propertyData[2].m_value);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(1u), propertyData[2].m_min);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(0xFFFFFF00u), propertyData[2].m_max);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(2u), propertyData[2].m_step);

        EXPECT_FALSE(loadResult.ContainsOutcome(JsonSerializationResult::Outcomes::Skipped));
    }

    TEST_F(MaterialPropertySerializerTests, LoadAndStoreJson_NonNumericType_AllValues)
    {
        const AZStd::string inputJson = R"(
        [
            {
                "id": "testProperty1",
                "type": "Bool",
                "defaultValue": true
            },
            {
                "id": "testProperty2",
                "type": "Vector2",
                "defaultValue": [0.1, 0.2]
            },
            {
                "id": "testProperty3",
                "type": "Vector3",
                "defaultValue": [0.3, 0.4, 0.5]
            },
            {
                "id": "testProperty4",
                "type": "Vector4",
                "defaultValue": [0.6, 0.5, 0.8, 0.4]
            },
            {
                "id": "testProperty5",
                "type": "Color",
                "defaultValue": [0.1, 0.2, 0.3]
            },
            {
                "id": "testProperty6",
                "type": "Image",
                "defaultValue": "Default.png"
            }
        ]
        )";

        AZStd::vector<MaterialTypeSourceData::PropertyDefinition> propertyData;
        JsonTestResult loadResult = LoadTestDataFromJson(propertyData, inputJson);

        EXPECT_EQ(AZ::JsonSerializationResult::Tasks::ReadField, loadResult.m_jsonResultCode.GetTask());
        EXPECT_EQ(AZ::JsonSerializationResult::Processing::Completed, loadResult.m_jsonResultCode.GetProcessing());

        EXPECT_EQ(MaterialPropertyDataType::Bool, propertyData[0].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(true), propertyData[0].m_value);

        EXPECT_EQ(MaterialPropertyDataType::Vector2, propertyData[1].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(AZ::Vector2{0.1f, 0.2f}), propertyData[1].m_value);

        EXPECT_EQ(MaterialPropertyDataType::Vector3, propertyData[2].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(AZ::Vector3{0.3f, 0.4f, 0.5f}), propertyData[2].m_value);

        EXPECT_EQ(MaterialPropertyDataType::Vector4, propertyData[3].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(AZ::Vector4{0.6f, 0.5f, 0.8f, 0.4f}), propertyData[3].m_value);

        EXPECT_EQ(MaterialPropertyDataType::Color, propertyData[4].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(AZ::Color{0.1f, 0.2f, 0.3f, 1.0f}), propertyData[4].m_value);

        EXPECT_EQ(MaterialPropertyDataType::Image, propertyData[5].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(AZStd::string{"Default.png"}), propertyData[5].m_value);

        for (int i = 0; i < propertyData.size(); ++i)
        {
            AZStd::string prefix = AZStd::string::format("/%d", i);
            EXPECT_TRUE(loadResult.ContainsMessage(prefix + "/id", "Success"));
            EXPECT_TRUE(loadResult.ContainsMessage(prefix + "/type", "Success"));
            EXPECT_TRUE(loadResult.ContainsMessage(prefix + "/defaultValue", "Success"));
        }

        EXPECT_FALSE(loadResult.ContainsOutcome(JsonSerializationResult::Outcomes::Skipped));

        TestStoreToJson(propertyData, inputJson);
    }

    TEST_F(MaterialPropertySerializerTests, LoadAndStoreJson_NonNumericType_DefaultValues)
    {
        const AZStd::string inputJson = R"(
        [
            {
                "id": "testProperty1",
                "type": "Bool"
            },
            {
                "id": "testProperty2",
                "type": "Vector2"
            },
            {
                "id": "testProperty3",
                "type": "Vector3"
            },
            {
                "id": "testProperty4",
                "type": "Vector4"
            },
            {
                "id": "testProperty5",
                "type": "Color"
            },
            {
                "id": "testProperty6",
                "type": "Image"
            }
        ]
        )";

        AZStd::vector<MaterialTypeSourceData::PropertyDefinition> propertyData;
        JsonTestResult loadResult = LoadTestDataFromJson(propertyData, inputJson);

        EXPECT_EQ(AZ::JsonSerializationResult::Tasks::ReadField, loadResult.m_jsonResultCode.GetTask());
        EXPECT_EQ(AZ::JsonSerializationResult::Processing::Completed, loadResult.m_jsonResultCode.GetProcessing());
        EXPECT_EQ(AZ::JsonSerializationResult::Outcomes::PartialDefaults, loadResult.m_jsonResultCode.GetOutcome());

        EXPECT_EQ(MaterialPropertyDataType::Bool, propertyData[0].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(false), propertyData[0].m_value);

        EXPECT_EQ(MaterialPropertyDataType::Vector2, propertyData[1].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(AZ::Vector2{0.0f, 0.0f}), propertyData[1].m_value);

        EXPECT_EQ(MaterialPropertyDataType::Vector3, propertyData[2].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(AZ::Vector3{0.0f, 0.0f, 0.0f}), propertyData[2].m_value);

        EXPECT_EQ(MaterialPropertyDataType::Vector4, propertyData[3].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(AZ::Vector4{0.0f, 0.0f, 0.0f, 0.0f}), propertyData[3].m_value);

        EXPECT_EQ(MaterialPropertyDataType::Color, propertyData[4].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(AZ::Color{1.0f, 1.0f, 1.0f, 1.0f}), propertyData[4].m_value);

        EXPECT_EQ(MaterialPropertyDataType::Image, propertyData[5].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(AZStd::string{""}), propertyData[5].m_value);

        EXPECT_FALSE(loadResult.ContainsOutcome(JsonSerializationResult::Outcomes::Skipped));

        TestStoreToJson(propertyData, inputJson);
    }

    TEST_F(MaterialPropertySerializerTests, Load_NonNumericType_AlternateValueRepresentation)
    {
        const AZStd::string inputJson = R"(
        [
            {
                "id": "testProperty1",
                "type": "Bool",
                "defaultValue": 1
            },
            {
                "id": "testProperty2",
                "type": "Vector2",
                "defaultValue": { "x": 0.4, "y": 0.1 }
            },
            {
                "id": "testProperty3",
                "type": "Vector3",
                "defaultValue": { "x": 0.4, "y": 0.1, "z": 0.5 }
            },
            {
                "id": "testProperty4",
                "type": "Vector4",
                "defaultValue": { "x": 0.4, "y": 0.1, "z": 0.5, "w": 0.6 }
            },
            {
                "id": "testProperty5",
                "type": "Color",
                "defaultValue": { "hex": "FF00FF" }
            }
        ]
        )";

        AZStd::vector<MaterialTypeSourceData::PropertyDefinition> propertyData;
        JsonTestResult loadResult = LoadTestDataFromJson(propertyData, inputJson);

        EXPECT_EQ(AZ::JsonSerializationResult::Tasks::ReadField, loadResult.m_jsonResultCode.GetTask());
        EXPECT_EQ(AZ::JsonSerializationResult::Processing::Completed, loadResult.m_jsonResultCode.GetProcessing());

        EXPECT_EQ(MaterialPropertyDataType::Bool, propertyData[0].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(true), propertyData[0].m_value);

        EXPECT_EQ(MaterialPropertyDataType::Vector2, propertyData[1].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(AZ::Vector2{0.4f, 0.1f}), propertyData[1].m_value);

        EXPECT_EQ(MaterialPropertyDataType::Vector3, propertyData[2].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(AZ::Vector3{0.4f, 0.1f, 0.5f}), propertyData[2].m_value);

        EXPECT_EQ(MaterialPropertyDataType::Vector4, propertyData[3].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(AZ::Vector4{0.4f, 0.1f, 0.5f, 0.6f}), propertyData[3].m_value);

        EXPECT_EQ(MaterialPropertyDataType::Color, propertyData[4].m_dataType);
        EXPECT_EQ(AZ::RPI::MaterialPropertyValue(AZ::Color{1.0f, 0.0f, 1.0f, 1.0f}), propertyData[4].m_value);

        EXPECT_FALSE(loadResult.ContainsOutcome(JsonSerializationResult::Outcomes::Skipped));
    }

    TEST_F(MaterialPropertySerializerTests, LoadAndStoreJson_OneConnection)
    {
        const AZStd::string inputJson = R"(
        {
            "id": "testProperty",
            "type": "Float",
            "connection": {
                "type": "ShaderOption",
                "id": "o_foo",
                "shaderIndex": 2
            }
        }
        )";

        MaterialTypeSourceData::PropertyDefinition propertyData;
        JsonTestResult loadResult = LoadTestDataFromJson(propertyData, inputJson);

        EXPECT_EQ(AZ::JsonSerializationResult::Tasks::ReadField, loadResult.m_jsonResultCode.GetTask());
        EXPECT_EQ(AZ::JsonSerializationResult::Processing::Completed, loadResult.m_jsonResultCode.GetProcessing());

        EXPECT_EQ(1, propertyData.m_outputConnections.size());
        EXPECT_EQ(MaterialPropertyOutputType::ShaderOption, propertyData.m_outputConnections[0].m_type);
        EXPECT_EQ("o_foo", propertyData.m_outputConnections[0].m_nameId);
        EXPECT_EQ(2, propertyData.m_outputConnections[0].m_shaderIndex);

        EXPECT_TRUE(loadResult.ContainsMessage("/connection/type", "Success"));
        EXPECT_TRUE(loadResult.ContainsMessage("/connection/id", "Success"));
        EXPECT_TRUE(loadResult.ContainsMessage("/connection/shaderIndex", "Success"));
        EXPECT_FALSE(loadResult.ContainsOutcome(JsonSerializationResult::Outcomes::Skipped));

        TestStoreToJson(propertyData, inputJson);
    }

    TEST_F(MaterialPropertySerializerTests, LoadAndStoreJson_MultipleConnections)
    {
        const AZStd::string inputJson = R"(
        {
            "id": "testProperty",
            "type": "Float",
            "connection": [
                {
                    "type": "ShaderInput",
                    "id": "o_foo",
                    "shaderIndex": 2
                },
                {
                    "type": "ShaderOption",
                    "id": "o_bar",
                    "shaderIndex": 1
                }
            ]
        }
        )";

        MaterialTypeSourceData::PropertyDefinition propertyData;
        JsonTestResult loadResult = LoadTestDataFromJson(propertyData, inputJson);

        EXPECT_EQ(AZ::JsonSerializationResult::Tasks::ReadField, loadResult.m_jsonResultCode.GetTask());
        EXPECT_EQ(AZ::JsonSerializationResult::Processing::Completed, loadResult.m_jsonResultCode.GetProcessing());

        EXPECT_EQ(2, propertyData.m_outputConnections.size());
        EXPECT_EQ(MaterialPropertyOutputType::ShaderInput, propertyData.m_outputConnections[0].m_type);
        EXPECT_EQ("o_foo", propertyData.m_outputConnections[0].m_nameId);
        EXPECT_EQ(2, propertyData.m_outputConnections[0].m_shaderIndex);

        EXPECT_EQ(MaterialPropertyOutputType::ShaderOption, propertyData.m_outputConnections[1].m_type);
        EXPECT_EQ("o_bar", propertyData.m_outputConnections[1].m_nameId);
        EXPECT_EQ(1, propertyData.m_outputConnections[1].m_shaderIndex);

        EXPECT_TRUE(loadResult.ContainsMessage("/connection/0/type", "Success"));
        EXPECT_TRUE(loadResult.ContainsMessage("/connection/0/id", "Success"));
        EXPECT_TRUE(loadResult.ContainsMessage("/connection/0/shaderIndex", "Success"));
        EXPECT_TRUE(loadResult.ContainsMessage("/connection/1/type", "Success"));
        EXPECT_TRUE(loadResult.ContainsMessage("/connection/1/id", "Success"));
        EXPECT_TRUE(loadResult.ContainsMessage("/connection/1/shaderIndex", "Success"));
        EXPECT_FALSE(loadResult.ContainsOutcome(JsonSerializationResult::Outcomes::Skipped));

        TestStoreToJson(propertyData, inputJson);
    }

    TEST_F(MaterialPropertySerializerTests, Load_Warning_SkippedTopLevelField)
    {
        // "conection" is misspelled
        const AZStd::string inputJson = R"(
        {
            "id": "testProperty",
            "type": "Float",
            "conection": [
                {
                    "type": "ShaderInput",
                    "id": "o_foo",
                    "shaderIndex": 2
                }
            ]
        }
        )";

        MaterialTypeSourceData::PropertyDefinition propertyData;
        JsonTestResult loadResult = LoadTestDataFromJson(propertyData, inputJson);

        EXPECT_EQ(AZ::JsonSerializationResult::Tasks::ReadField, loadResult.m_jsonResultCode.GetTask());
        EXPECT_EQ(AZ::JsonSerializationResult::Processing::Completed, loadResult.m_jsonResultCode.GetProcessing());

        EXPECT_EQ(propertyData.m_nameId, "testProperty");
        EXPECT_EQ(propertyData.m_dataType, MaterialPropertyDataType::Float);
        EXPECT_EQ(propertyData.m_outputConnections.size(), 0);

        EXPECT_TRUE(loadResult.ContainsMessage("/conection", "skip"));
    }

    TEST_F(MaterialPropertySerializerTests, Load_Warning_SkippedConnectionField)
    {
        // "shadrIndex" is misspelled
        const AZStd::string inputJson = R"(
        {
            "id": "testProperty",
            "type": "Float",
            "connection": [
                {
                    "type": "ShaderInput",
                    "shadrIndex": 2,
                    "id": "o_foo"
                }
            ]
        }
        )";

        MaterialTypeSourceData::PropertyDefinition propertyData;
        JsonTestResult loadResult = LoadTestDataFromJson(propertyData, inputJson);

        EXPECT_EQ(AZ::JsonSerializationResult::Tasks::ReadField, loadResult.m_jsonResultCode.GetTask());
        EXPECT_EQ(AZ::JsonSerializationResult::Processing::Completed, loadResult.m_jsonResultCode.GetProcessing());

        EXPECT_EQ(propertyData.m_nameId, "testProperty");
        EXPECT_EQ(propertyData.m_dataType, MaterialPropertyDataType::Float);
        EXPECT_EQ(propertyData.m_outputConnections.size(), 1);
        EXPECT_EQ(propertyData.m_outputConnections[0].m_nameId, "o_foo");
        EXPECT_EQ(propertyData.m_outputConnections[0].m_type, MaterialPropertyOutputType::ShaderInput);
        EXPECT_EQ(propertyData.m_outputConnections[0].m_shaderIndex, -1);

        EXPECT_TRUE(loadResult.ContainsMessage("/connection/0/shadrIndex", "skip"));
    }
}

