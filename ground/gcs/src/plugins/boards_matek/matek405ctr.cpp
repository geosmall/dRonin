/**
 ******************************************************************************
 * @file       matek405ctr.cpp
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2013
 * @author     dRonin, http://dronin.org Copyright (C) 2015
 *
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup Boards_MatekPlugin Matek boards support Plugin
 * @{
 * @brief Plugin to support Matek boards
 *****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>
 */

#include "matek405ctr.h"

#include "uavobjects/uavobjectmanager.h"
#include "uavobjectutil/uavobjectutilmanager.h"
#include <extensionsystem/pluginmanager.h>
#include "board_usb_ids.h"

#include "hwmatek405ctr.h"

/**
 * @brief MATEK405CTR::MATEK405CTR
 *  This is the MATEK405CTR board definition
 */
MATEK405CTR::MATEK405CTR(void)
{
    // Common USB IDs
    addBootloaderUSBInfo(
        USBInfo(DRONIN_VID_DRONIN_BOOTLOADER, DRONIN_PID_DRONIN_BOOTLOADER, BCD_DEVICE_BOOTLOADER));
    addFirmwareUSBInfo(
        USBInfo(DRONIN_VID_DRONIN_FIRMWARE, DRONIN_PID_DRONIN_FIRMWARE, BCD_DEVICE_FIRMWARE));
    
    boardType = 0x95;

    // Define the bank of channels that are connected to a given timer
    channelBanks.resize(2);
    channelBanks[0] = QVector<int>() << 1;
    channelBanks[1] = QVector<int>() << 2<< 3 << 4;
}

MATEK405CTR::~MATEK405CTR()
{
}

QString MATEK405CTR::shortName()
{
    return QString("MATEK405CTR");
}

QString MATEK405CTR::boardDescription()
{
    return QString("The MATEK405CTR board");
}

//! Return which capabilities this board has
bool MATEK405CTR::queryCapabilities(BoardCapabilities capability)
{
    switch (capability) {
    case BOARD_CAPABILITIES_GYROS:
    case BOARD_CAPABILITIES_ACCELS:
    case BOARD_CAPABILITIES_MAGS:
    case BOARD_CAPABILITIES_BAROS:
    case BOARD_CAPABILITIES_UPGRADEABLE:
        return true;
    default:
        break;
    }

    return false;
}

QPixmap MATEK405CTR::getBoardPicture()
{
    return QPixmap(":/matek/images/matek405ctr.png");
}

//! Determine if this board supports configuring the receiver
bool MATEK405CTR::isInputConfigurationSupported(Core::IBoardType::InputType type)
{
    switch (type) {
    case INPUT_TYPE_PWM:
    case INPUT_TYPE_UNKNOWN:
        return false;
    default:
        break;
    }

    return true;
}

QString MATEK405CTR::getHwUAVO()
{
    return "HwMatek405Ctr";
}

/**
 * Configure the board to use a receiver input type on a port number
 * @param type the type of receiver to use
 * @return true if successfully configured or false otherwise
 */
bool MATEK405CTR::setInputType(Core::IBoardType::InputType type)
{
    ExtensionSystem::PluginManager *pm = ExtensionSystem::PluginManager::instance();
    UAVObjectManager *uavoManager = pm->getObject<UAVObjectManager>();
    HwMatek405Ctr *hwMatek405Ctr = HwMatek405Ctr::GetInstance(uavoManager);
    Q_ASSERT(hwMatek405Ctr);
    if (!hwMatek405Ctr)
        return false;

    HwMatek405Ctr::DataFields settings = hwMatek405Ctr->getData();

    switch (type) {
    case INPUT_TYPE_PPM:
        settings.PPM_Receiver = HwMatek405Ctr::PPM_RECEIVER_ENABLED;
        break;
    case INPUT_TYPE_HOTTSUMD:
        settings.Uart2 = HwMatek405Ctr::UART2_HOTTSUMD;
        break;
    case INPUT_TYPE_HOTTSUMH:
        settings.Uart2 = HwMatek405Ctr::UART2_HOTTSUMH;
        break;
    case INPUT_TYPE_SBUS:
        settings.Uart2 = HwMatek405Ctr::UART2_SBUS;
        break;
    case INPUT_TYPE_SBUSNONINVERTED:
        settings.Uart2 = HwMatek405Ctr::UART2_SBUSNONINVERTED;
        break;
    case INPUT_TYPE_IBUS:
        settings.Uart2 = HwMatek405Ctr::UART2_IBUS;
        break;
    case INPUT_TYPE_DSM:
        settings.Uart2 = HwMatek405Ctr::UART2_DSM;
        break;
    case INPUT_TYPE_SRXL:
        settings.Uart2 = HwMatek405Ctr::UART2_SRXL;
        break;
    case INPUT_TYPE_TBSCROSSFIRE:
        settings.Uart2 = HwMatek405Ctr::UART2_TBSCROSSFIRE;
        break;
    default:
        return false;
    }

    // Apply these changes
    hwMatek405Ctr->setData(settings);

    return true;
}

/**
 * @brief MATEK405CTR::getInputType fetch the currently selected input type
 * @return the selected input type
 */
Core::IBoardType::InputType MATEK405CTR::getInputType()
{
    ExtensionSystem::PluginManager *pm = ExtensionSystem::PluginManager::instance();
    UAVObjectManager *uavoManager = pm->getObject<UAVObjectManager>();
    HwMatek405Ctr *hwMatek405Ctr = HwMatek405Ctr::GetInstance(uavoManager);
    Q_ASSERT(hwMatek405Ctr);
    if (!hwMatek405Ctr)
        return INPUT_TYPE_UNKNOWN;

    HwMatek405Ctr::DataFields settings = hwMatek405Ctr->getData();

    switch (settings.PPM_Receiver) {
    case HwMatek405Ctr::PPM_RECEIVER_ENABLED:
        return INPUT_TYPE_PPM;
    default:
        break;
    }

    switch (settings.Uart2) {
    case HwMatek405Ctr::UART2_HOTTSUMD:
        return INPUT_TYPE_HOTTSUMD;
    case HwMatek405Ctr::UART2_HOTTSUMH:
        return INPUT_TYPE_HOTTSUMH;
    case HwMatek405Ctr::UART2_SBUS:
        return INPUT_TYPE_SBUS;
    case HwMatek405Ctr::UART2_SBUSNONINVERTED:
        return INPUT_TYPE_SBUSNONINVERTED;
    case HwMatek405Ctr::UART2_IBUS:
        return INPUT_TYPE_IBUS;
    case HwMatek405Ctr::UART2_SRXL:
        return INPUT_TYPE_SRXL;
    case HwMatek405Ctr::UART2_TBSCROSSFIRE:
        return INPUT_TYPE_TBSCROSSFIRE;
    default:
        break;
    }

    return INPUT_TYPE_UNKNOWN;
}

int MATEK405CTR::queryMaxGyroRate()
{
    ExtensionSystem::PluginManager *pm = ExtensionSystem::PluginManager::instance();
    UAVObjectManager *uavoManager = pm->getObject<UAVObjectManager>();
    HwMatek405Ctr *hwMatek405Ctr = HwMatek405Ctr::GetInstance(uavoManager);
    Q_ASSERT(hwMatek405Ctr);
    if (!hwMatek405Ctr)
        return 0;

    HwMatek405Ctr::DataFields settings = hwMatek405Ctr->getData();

    switch (settings.GyroRange) {
    case HwMatek405Ctr::GYRORANGE_250:
        return 250;
    case HwMatek405Ctr::GYRORANGE_500:
        return 500;
    case HwMatek405Ctr::GYRORANGE_1000:
        return 1000;
    case HwMatek405Ctr::GYRORANGE_2000:
        return 2000;
    default:
        break;
    }

    return 500;
}

QStringList MATEK405CTR::getAdcNames()
{
    return QStringList() << "Current" << "Voltage" << "RSSI";
}

bool MATEK405CTR::hasAnnunciator(AnnunciatorType annunc)
{
    switch (annunc) {
    case ANNUNCIATOR_HEARTBEAT:
    case ANNUNCIATOR_ALARM:
    case ANNUNCIATOR_BUZZER:
    case ANNUNCIATOR_RGB:
	case ANNUNCIATOR_DAC:
	    return true;
	}
		return false;
}