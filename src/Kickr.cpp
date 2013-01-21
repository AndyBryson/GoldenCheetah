/*
 * Copyright (c) 2013 Mark Liversedge (liversedge@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "Kickr.h"

/* ----------------------------------------------------------------------
 * CONSTRUCTOR/DESRTUCTOR
 * ---------------------------------------------------------------------- */
Kickr::Kickr(QObject *parent,  DeviceConfiguration *devConf) : QThread(parent)
{
    this->parent = parent;
    this->devConf = devConf;
}

Kickr::~Kickr()
{
}

/* ----------------------------------------------------------------------
 * SET
 * ---------------------------------------------------------------------- */
void Kickr::setDevice(QString)
{
    // not required
}

void Kickr::setMode(int mode, double load, double gradient)
{
    pvars.lock();
    this->mode = mode;
    this->load = load;
    this->gradient = gradient;
    pvars.unlock();
}

void Kickr::setLoad(double load)
{
    pvars.lock();
    if (load > 1500) load = 1500;
    if (load < 50) load = 50;
    this->load = load;
    pvars.unlock();
}

void Kickr::setGradient(double gradient)
{
    pvars.lock();
    this->gradient = gradient;
    pvars.unlock();
}


/* ----------------------------------------------------------------------
 * GET
 * ---------------------------------------------------------------------- */

int Kickr::getMode()
{
    int  tmp;
    pvars.lock();
    tmp = mode;
    pvars.unlock();
    return tmp;
}

double Kickr::getLoad()
{
    double tmp;
    pvars.lock();
    tmp = load;
    pvars.unlock();
    return tmp;
}

double Kickr::getGradient()
{
    double tmp;
    pvars.lock();
    tmp = gradient;
    pvars.unlock();
    return tmp;
}

void
Kickr::getRealtimeData(RealtimeData &rtData)
{
    rtData.setWatts(0); // XXX watts only...
}

int
Kickr::start()
{
    QThread::start();
    return 0;
}

// does nothing - neither does pause
int Kickr::restart() { return 0; }
int Kickr::pause() { return 0; }

int Kickr::stop()
{
    running = false;
    return 0;
}

// used by thread to set variables and emit event if needed
// on unexpected exit
int Kickr::quit(int code)
{
    // event code goes here!
    exit(code);
    return 0; // never gets here obviously but shuts up the compiler!
}

/*----------------------------------------------------------------------
 * THREADED CODE - READS TELEMETRY AND SENDS COMMANDS TO KEEP CT ALIVE
 *----------------------------------------------------------------------*/
void Kickr::run()
{
    // Connect to the device
    if (connectKickr()) {
        quit(2);
        return; // open failed!
    }

    running = true;
    while(running) {

        // send load
        //XXX todo

        msleep(10);
    }

    disconnectKickr();

    quit(0);
}

bool
Kickr::find()
{
    WFApi *w = WFApi::getInstance();

    if (w->discoverDevicesOfType(1,1,1) == false) return false;

    QEventLoop loop;
    connect(w, SIGNAL(discoveredDevices(int,bool)), &loop, SLOT(quit()));
    loop.exec();

    if (w->deviceCount()) {
        deviceUUID = w->deviceUUID(0);
        return true;
    } else return false;
}

int
Kickr::connectKickr()
{
    // connect

    //failed
    connected = false;
    return -1;
}

int
Kickr::disconnectKickr()
{
    // disconnect

    connected = false;
    return 0;
}

// check to see of there is a port at the device specified
// returns true if the device exists and false if not
bool Kickr::discover(QString)
{
    return false;
}