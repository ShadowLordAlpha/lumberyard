/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
#include "StdAfx.h"

#include <Editor/AssetEditor/AssetEditorWindow.h>
#include <AssetEditor/ui_AssetEditorWindow.h>

#include <AzCore/Asset/AssetManager.h>
#include <AzCore/IO/FileIO.h>

#include <AzToolsFramework/AssetEditor/AssetEditorWidget.h>

#include <QPushButton>
#include <QMessageBox>
#include <AzCore/UserSettings/UserSettingsComponent.h>

namespace AssetEditorUtils
{
    AssetEditorWindow* CreateAssetEditorWithAsset(const AZ::Data::Asset<AZ::Data::AssetData> assetRef)
    {
        AssetEditorWindow* assetEditorWindow = new AssetEditorWindow();
        if (auto assetsWindowsToRestore = AZ::UserSettings::CreateFind<AzToolsFramework::AssetEditor::AssetEditorWindowSettings>(AZ::Crc32(AzToolsFramework::AssetEditor::AssetEditorWindowSettings::s_name), AZ::UserSettings::CT_GLOBAL))
        {
            assetsWindowsToRestore->m_openAssets.emplace(assetRef);
        }

        AZ::UserSettingsComponentRequestBus::Broadcast(&AZ::UserSettingsComponentRequestBus::Events::Save);

        auto&& loadedAsset = AZ::Data::AssetManager::Instance().GetAsset(assetRef.GetId(), assetRef.GetType(), true, &AZ::ObjectStream::AssetFilterDefault, true);
        assetEditorWindow->OpenAsset(loadedAsset);
        return assetEditorWindow;
    }
}


AssetEditorWindow::AssetEditorWindow(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::AssetEditorWindowClass())
{
    using namespace AzToolsFramework::AssetEditor;

    m_ui->setupUi(this);

    connect(m_ui->m_assetEditorWidget, &AssetEditorWidget::OnAssetSaveFailedSignal, this, &AssetEditorWindow::OnAssetSaveFailed);
    connect(m_ui->m_assetEditorWidget, &AssetEditorWidget::OnAssetOpenedSignal, this, &AssetEditorWindow::OnAssetOpened);

    BusConnect();
}

AssetEditorWindow::~AssetEditorWindow()
{
    using namespace AzToolsFramework::AssetEditor;
    BusDisconnect();
}

void AssetEditorWindow::RegisterViewClass(const AZ::Data::Asset<AZ::Data::AssetData>& asset)
{
    AzToolsFramework::ViewPaneOptions options;
    options.sendViewPaneNameBackToAmazonAnalyticsServers = true;
    options.showInMenu = false;
    auto& assetName = asset.GetHint();
    const char* paneName = assetName.c_str();
    AzToolsFramework::RegisterViewPane<AssetEditorWindow>(paneName, LyViewPane::CategoryTools, options, [asset](QWidget*) {return AssetEditorUtils::CreateAssetEditorWithAsset(asset); });
}

void AssetEditorWindow::CreateAsset(const AZ::Data::AssetType& assetType)
{
    m_ui->m_assetEditorWidget->CreateAsset(assetType);
}

void AssetEditorWindow::OpenAsset(const AZ::Data::Asset<AZ::Data::AssetData>& asset)
{
    m_ui->m_assetEditorWidget->OpenAsset(asset);
}

void AssetEditorWindow::RegisterViewClass()
{
    AzToolsFramework::ViewPaneOptions options;
    options.preferedDockingArea = Qt::LeftDockWidgetArea;
    options.sendViewPaneNameBackToAmazonAnalyticsServers = true;
    AzToolsFramework::RegisterViewPane<AssetEditorWindow>(LyViewPane::AssetEditor, LyViewPane::CategoryTools, options);
}

void AssetEditorWindow::OnAssetOpened(const AZ::Data::Asset<AZ::Data::AssetData>& asset)
{
    if (asset)
    {
        AZStd::string assetPath;
        AZStd::string assetName;
        AZStd::string extension;
        AZ::Data::AssetCatalogRequestBus::BroadcastResult(assetPath, &AZ::Data::AssetCatalogRequests::GetAssetPathById, asset.GetId());
        AzFramework::StringFunc::Path::Split(assetPath.c_str(), nullptr, nullptr, &assetName, &extension);
//        AZStd::string windowTitle = AZStd::string::format("Edit Asset: %s", (assetName + extension).c_str());

        AZStd::string windowTitle = asset.GetHint();

        qobject_cast<QWidget*>(parent())->setWindowTitle(tr(windowTitle.c_str()));
    }
    else
    {
        qobject_cast<QWidget*>(parent())->setWindowTitle(tr("Asset Editor"));
    }
}

void AssetEditorWindow::closeEvent(QCloseEvent* event)
{
    if (m_ui->m_assetEditorWidget->WaitingToSave())
    {
        // Don't need to ask to save, as a save is already queued.
        m_ui->m_assetEditorWidget->SetCloseAfterSave();
        event->ignore();
        return;
    }

    if (m_ui->m_assetEditorWidget->TrySave([this]() {  qobject_cast<QWidget*>(parent())->close(); }))
    {
        event->ignore();
    }
}

void AssetEditorWindow::OnAssetSaveFailed(const AZStd::string& error)
{
    QMessageBox::warning(this, tr("Unable to Save Asset"),
        tr(error.c_str()), QMessageBox::Ok, QMessageBox::Ok);
}

#include <AssetEditor/AssetEditorWindow.moc>
