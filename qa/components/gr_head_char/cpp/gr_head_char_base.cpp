/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file 
 * distributed with this source distribution.
 * 
 * This file is part of GNUHAWK.
 * 
 * GNUHAWK is free software: you can redistribute it and/or modify is under the 
 * terms of the GNU General Public License as published by the Free Software 
 * Foundation, either version 3 of the License, or (at your option) any later 
 * version.
 * 
 * GNUHAWK is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
 * details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program.  If not, see http://www.gnu.org/licenses/.
 */


#include "gr_head_char_base.h"

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY
    
 	Source: gr_head_char.spd.xml
 	Generated on: Fri Feb 08 19:03:20 EST 2013
 	Redhawk IDE
 	Version:R.1.8.2
 	Build id: v201212041901

*******************************************************************************************/

/******************************************************************************************

    The following class functions are for the base class for the component class. To
    customize any of these functions, do not modify them here. Instead, overload them
    on the child class

******************************************************************************************/
 
gr_head_char_base::gr_head_char_base(const char *uuid, const char *label) :
                                     Resource_impl(uuid, label), serviceThread(0) {
    construct();
}

void gr_head_char_base::construct()
{
    Resource_impl::_started = false;
    loadProperties();
    serviceThread = 0;
    
    PortableServer::ObjectId_var oid;
    char_in = new BULKIO_dataChar_In_i("char_in", this);
    oid = ossie::corba::RootPOA()->activate_object(char_in);
    char_out = new BULKIO_dataChar_Out_i("char_out", this);
    oid = ossie::corba::RootPOA()->activate_object(char_out);

    registerInPort(char_in);
    registerOutPort(char_out, char_out->_this());
}

/*******************************************************************************************
    Framework-level functions
    These functions are generally called by the framework to perform housekeeping.
*******************************************************************************************/
void gr_head_char_base::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException)
{
}

void gr_head_char_base::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    boost::mutex::scoped_lock lock(serviceThreadLock);
    if (serviceThread == 0) {
        char_in->unblock();
        serviceThread = new ProcessThread<gr_head_char_base>(this, 0.1);
        serviceThread->start();
    }
    
    if (!Resource_impl::started()) {
    	Resource_impl::start();
    }
}

void gr_head_char_base::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    boost::mutex::scoped_lock lock(serviceThreadLock);
    // release the child thread (if it exists)
    if (serviceThread != 0) {
        char_in->block();
        if (!serviceThread->release(2)) {
            throw CF::Resource::StopError(CF::CF_NOTSET, "Processing thread did not die");
        }
        serviceThread = 0;
    }
    
    if (Resource_impl::started()) {
    	Resource_impl::stop();
    }
}

CORBA::Object_ptr gr_head_char_base::getPort(const char* _id) throw (CORBA::SystemException, CF::PortSupplier::UnknownPort)
{

    std::map<std::string, Port_Provides_base_impl *>::iterator p_in = inPorts.find(std::string(_id));
    if (p_in != inPorts.end()) {

        if (!strcmp(_id,"char_in")) {
            BULKIO_dataChar_In_i *ptr = dynamic_cast<BULKIO_dataChar_In_i *>(p_in->second);
            if (ptr) {
                return BULKIO::dataChar::_duplicate(ptr->_this());
            }
        }
    }

    std::map<std::string, CF::Port_var>::iterator p_out = outPorts_var.find(std::string(_id));
    if (p_out != outPorts_var.end()) {
        return CF::Port::_duplicate(p_out->second);
    }

    throw (CF::PortSupplier::UnknownPort());
}

void gr_head_char_base::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    // This function clears the component running condition so main shuts down everything
    try {
        stop();
    } catch (CF::Resource::StopError& ex) {
        // TODO - this should probably be logged instead of ignored
    }

    // deactivate ports
    releaseInPorts();
    releaseOutPorts();

    delete(char_in);
    delete(char_out);
 
    Resource_impl::releaseObject();
}

void gr_head_char_base::loadProperties()
{
    addProperty(num,
                0, 
               "num",
               "",
               "readwrite",
               "",
               "external",
               "configure");

    addProperty(done,
                false, 
               "done",
               "",
               "readwrite",
               "",
               "external",
               "configure");

}
