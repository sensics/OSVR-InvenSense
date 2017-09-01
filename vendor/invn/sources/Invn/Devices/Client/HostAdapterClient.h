//-----------------------------------------------------------------------------
/*
 Copyright © 2015 InvenSense Inc. All rights reserved.

 This software, related documentation and any modifications thereto collectively “Software” is subject
 to InvenSense and its licensors' intellectual property rights under U.S. and international copyright and
 other intellectual property rights laws.

 InvenSense and its licensors retain all intellectual property and proprietary rights in and to the Software
 and any use, reproduction, disclosure or distribution of the Software without an express license
 agreement from InvenSense is strictly prohibited.
 */
//-----------------------------------------------------------------------------

#ifndef _IDD_HostAdapterClient_h_
#define _IDD_HostAdapterClient_h_

#include "Invn/InvExport.h"

#include "Invn/Devices/SerifHal.h"
#include "Invn/Devices/HostSerif.h" // legacy
#include "Invn/Devices/HostAdapter/HostAdapter.h"

// STL
#include <map>
#include <string>
#include <exception>

//-----------------------------------------------------------------------------

class INV_EXPORT HostAdapterClient
{
public:
    typedef std::map<std::string,std::string> OptionMap;
    class OpenException : public std::exception { virtual const char *what() const throw(); };
    class NotFoundException : public std::exception { virtual const char *what() const throw(); };
    class ProtocolException : public std::exception { virtual const char *what() const throw(); };
    class UnknowOptionException : public std::exception { virtual const char *what() const throw(); };

    /// Constructor.
    /// You must call open() after constructing this object.
    /// Will set HostAdaptaer::Singleton instance
    /// @param serif    Reference to host adapter implementation.
    ///                 Default is #AutoSelectAdapter in order to select
    ///                     the first kind of connected adapter.
    HostAdapterClient(HostAdapter * adapter);
    virtual ~HostAdapterClient();

    void open();
    void close();
    bool isOpened() const { return _opened; }

    /// Retrieve the internal handle of the host adapter.
    const inv_serif_hal_t* getSerifHandle() const { return &_serif; }

    /// Retrieve the internal handle of the host adapter.
    const inv_host_serif_t* getHandle() const;

    /// Retrieve the name of the adapter (after it was opened).
    /// @throw std::runtime_error if not opened yet.
    virtual const char* getName() const = 0;

    /// Generic function to set an integer option
    /// @throw UnknowOptionException if not managed by this adapter
    void setOption(const std::string& sOptionName, int iValue);

    /// Generic function to set a string option
    /// @throw UnknowOptionException if not managed by this adapter
    void setOption(const std::string& sOptionName, const std::string& sValue);

    /// Helper to create an HostAdapterClient
    /// by auto-selecting the first one that is plugged.
    /// @return Reference to plugged HostAdapterClient
    ///        (this one is currently already opened),
    ///         or 0 if not found.
    static HostAdapterClient* autoSelect();

    /// Helper to create an HostAdapterClient
    /// according to adapter name and string options.
    ///
    /// Possible adapters name & options:\n
    /// - aardvarki2c (options: speed=<speed-khz>, addr=<addr-hex>)"
    /// - cheetah     (options: speed=<speed-khz>, uid=<uid>)"
    /// - spibridge   (options: port=<com-port>)"
    /// - ft4222spi   (options: speed=<speed-khz>)"
    static HostAdapterClient* factoryCreate(const std::string& sAdapterName, const OptionMap& options = OptionMap());

protected:
    void buildHostSerif();

    HostAdapter *   _handle;
    inv_serif_hal_t _serif;
    inv_host_serif_t _host_serif;
    bool _opened;
};

//-----------------------------------------------------------------------------
// keep retro-compatibilty with HostAdapterClientArduino class
typedef class HostAdapterClientUartSpiBridge HostAdapterClientArduino;

class INV_EXPORT HostAdapterClientUartSpiBridge : public HostAdapterClient
{
public:
    HostAdapterClientUartSpiBridge();
    const char* getName() const;
    void setComPort(const std::string& port);
};

//-----------------------------------------------------------------------------

class INV_EXPORT HostAdapterClientAardvark : public HostAdapterClient
{
public:
    HostAdapterClientAardvark();
    const char* getName() const;
    void setSpeedOption(uint16_t clock);
    void setAddrOption(uint8_t addr);
    void setSlaveModeOption(uint8_t addr);
};

//-----------------------------------------------------------------------------

class INV_EXPORT HostAdapterClientCheetah : public HostAdapterClient
{
public:
    HostAdapterClientCheetah();
    const char* getName() const;
    void setSpeedOption(uint16_t clock);
    void setUidOption(uint32_t uid);
};

//-----------------------------------------------------------------------------

class INV_EXPORT HostAdapterClientFt4222Spi : public HostAdapterClient
{
public:
    HostAdapterClientFt4222Spi();
    const char* getName() const;
    void setSpeedOption(uint16_t clock);
};

//-----------------------------------------------------------------------------

class INV_EXPORT HostAdapterClientDummy : public HostAdapterClient
{
public:
    HostAdapterClientDummy();
    const char* getName() const;
};

//-----------------------------------------------------------------------------

#endif // _IDD_HostAdapterClient_h_
