/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include "MorphTargetEditWindow.h"
#include "../../../../EMStudioSDK/Source/EMStudioManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>


namespace EMStudio
{
    MorphTargetEditWindow::MorphTargetEditWindow(EMotionFX::ActorInstance* actorInstance, EMotionFX::MorphTarget* morphTarget, QWidget* parent)
        : QDialog(parent)
    {
        // keep values
        mActorInstance = actorInstance;
        mMorphTarget = morphTarget;

        // init the phoneme selection window
        mPhonemeSelectionWindow = nullptr;

        // set the window name
        setWindowTitle(QString("Edit Morph Target: %1").arg(morphTarget->GetName()));

        // create the layout
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setAlignment(Qt::AlignVCenter);

        // get the morph target range min/max
        const float morphTargetRangeMin = mMorphTarget->GetRangeMin();
        const float morphTargetRangeMax = mMorphTarget->GetRangeMax();

        // create the range min label
        QLabel* rangeMinLabel = new QLabel("Range Min");

        // create the range min double spinbox
        mRangeMin = new AzQtComponents::DoubleSpinBox();
        mRangeMin->setSingleStep(0.1);
        mRangeMin->setRange(std::numeric_limits<int32>::lowest(), morphTargetRangeMax);
        mRangeMin->setValue(morphTargetRangeMin);
        connect(mRangeMin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &MorphTargetEditWindow::MorphTargetRangeMinValueChanged);

        // create the range max label
        QLabel* rangeMaxLabel = new QLabel("Range Max");

        // create the range max double spinbox
        mRangeMax = new AzQtComponents::DoubleSpinBox();
        mRangeMax->setSingleStep(0.1);
        mRangeMax->setRange(morphTargetRangeMin, std::numeric_limits<int32>::max());
        mRangeMax->setValue(morphTargetRangeMax);
        connect(mRangeMax, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &MorphTargetEditWindow::MorphTargetRangeMaxValueChanged);

        // create the grid layout
        QGridLayout* gridLayout = new QGridLayout();
        gridLayout->addWidget(rangeMinLabel, 0, 0);
        gridLayout->addWidget(mRangeMin, 0, 1);
        gridLayout->addWidget(rangeMaxLabel, 1, 0);
        gridLayout->addWidget(mRangeMax, 1, 1);

        // create the buttons layout
        QHBoxLayout* buttonsLayout = new QHBoxLayout();
        buttonsLayout->setMargin(0);

        // create the OK button
        QPushButton* OKButton = new QPushButton("OK");
        connect(OKButton, &QPushButton::clicked, this, &MorphTargetEditWindow::Accepted);

        // create the cancel button
        QPushButton* cancelButton = new QPushButton("Cancel");
        connect(cancelButton, &QPushButton::clicked, this, &MorphTargetEditWindow::reject);

        // add widgets in the buttons layout
        buttonsLayout->addWidget(OKButton);
        buttonsLayout->addWidget(cancelButton);

        // add widgets in the layout
        layout->addLayout(gridLayout);
        layout->addLayout(buttonsLayout);

        // set the layout
        setLayout(layout);

        // set the default size
        resize(300, minimumHeight());
    }


    MorphTargetEditWindow::~MorphTargetEditWindow()
    {
        delete mPhonemeSelectionWindow;
    }


    void MorphTargetEditWindow::UpdateInterface()
    {
        // get the morph target range min/max
        const float morphTargetRangeMin = mMorphTarget->GetRangeMin();
        const float morphTargetRangeMax = mMorphTarget->GetRangeMax();

        // disable signals
        mRangeMin->blockSignals(true);
        mRangeMax->blockSignals(true);

        // update the range min
        mRangeMin->setRange(std::numeric_limits<int32>::lowest(), morphTargetRangeMax);
        mRangeMin->setValue(morphTargetRangeMin);

        // update the range max
        mRangeMax->setRange(morphTargetRangeMin, std::numeric_limits<int32>::max());
        mRangeMax->setValue(morphTargetRangeMax);

        // enable signals
        mRangeMin->blockSignals(false);
        mRangeMax->blockSignals(false);

        // update the phoneme selection window
        if (mPhonemeSelectionWindow)
        {
            mPhonemeSelectionWindow->UpdateInterface();
        }
    }


    void MorphTargetEditWindow::MorphTargetRangeMinValueChanged(double value)
    {
        const float rangeMin = (float)value;
        mRangeMax->setRange(rangeMin, std::numeric_limits<int32>::max());
    }


    void MorphTargetEditWindow::MorphTargetRangeMaxValueChanged(double value)
    {
        const float rangeMax = (float)value;
        mRangeMin->setRange(std::numeric_limits<int32>::lowest(), rangeMax);
    }


    void MorphTargetEditWindow::Accepted()
    {
        const float rangeMin = (float)mRangeMin->value();
        const float rangeMax = (float)mRangeMax->value();

        AZStd::string result;
        AZStd::string command = AZStd::string::format("AdjustMorphTarget -actorInstanceID %i -lodLevel %i -name \"%s\" -rangeMin %f -rangeMax %f", mActorInstance->GetID(), mActorInstance->GetLODLevel(), mMorphTarget->GetNameString().c_str(), rangeMin, rangeMax);
        if (EMStudio::GetCommandManager()->ExecuteCommand(command, result) == false)
        {
            AZ_Error("EMotionFX", false, result.c_str());
        }

        accept();
    }


    void MorphTargetEditWindow::EditPhonemeButtonClicked()
    {
        delete mPhonemeSelectionWindow;
        mPhonemeSelectionWindow =  new PhonemeSelectionWindow(mActorInstance->GetActor(), mActorInstance->GetLODLevel(), mMorphTarget, this);
        mPhonemeSelectionWindow->exec();
    }
} // namespace EMStudio

#include <EMotionFX/Tools/EMotionStudio/Plugins/StandardPlugins/Source/MorphTargetsWindow/moc_MorphTargetEditWindow.cpp>
