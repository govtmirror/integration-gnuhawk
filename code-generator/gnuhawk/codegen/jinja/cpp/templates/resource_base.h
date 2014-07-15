/*#
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
 #*/
/*{% extends "pull/resource_base.h" %}*/

//% set prefix = component.prefix
//% set hasOutput = component.hasbulkiouses
//% set hasInput = component.hasbulkioprovides
//% set inputType = component.inputType
//% set outputType = component.outputType
//% set mem_align = component.mem_align
//% set hasMultiOut = component.hasmultioutports

/*{% block license %}*/
${component.cppLicense}

/*{% endblock %}*/

/*######## inherit includeGuard from redhawk base template ###########*/

/*{% block includes %}*/
${super()}
#include <boost/date_time/posix_time/posix_time.hpp>
#include "${prefix}_GnuHawkBlock.h"
#include <sstream>
/*{% endblock %}*/

/*######## inherit processThreadConstructor from redhawk base template ###########*/
/*######## inherit processThreadStart from redhawk base template       ###########*/
/*######## inherit processThreadRelease from redhawk base template     ###########*/
/*######## inherit processThreadDestructor from redhawk base template  ###########*/
/*######## inherit processThreadUpdateDelay from redhawk base template ###########*/
/*######## inherit processThreadPrivateVars from redhawk base template ###########*/

/*{% block classPrototype %}*/
class ${component.baseclass.name} : public GnuHawkBlock, ThreadedComponent
/*{% endblock %}*/

/*######## inherit cfResource prototypes from redhawk base template ###########*/

/*{% block basePublicFunctions %}*/
        void loadProperties();
/*{% endblock %}*/

/*{% block baseProtectedMembers %}*/
${super()}
/*{% if hasInput %}*/
        std::vector< std::string > inputPortOrder;
/*{% endif %}*/
/*{% if hasOutput %}*/
        std::vector< std::string > outputPortOrder;
/*{% endif %}*/
/*{% endblock %}*/

/*{% block extensions %}*/
        void construct();
    protected:
        static       const        int                 RealMode=0;
        static       const        int                 ComplexMode=1;
        std::vector<std::vector<int> >                io_mapping;
        typedef      boost::posix_time::ptime         TimeMark;
        typedef      boost::posix_time::time_duration TimeDuration;
        typedef      BULKIO::PrecisionUTCTime         TimeStamp;

        //
        // Enable or disable to adjusting of timestamp based on output rate
        //  
        inline void maintainTimeStamp( bool onoff=false ) {
           _maintainTimeStamp = onoff;
        };

        //
        // Enable or disable throttling of processing
        //  
        inline void setThrottle( bool onoff=false ) {
           _throttle = onoff;
        };

        //
        // getTargetDuration
        //
        // Target duration defines the expected time the service function requires
        // to produce/consume elements. For source patterns, the data output rate
        // will be used to defined the target duration.  For sink patterns, the
        // input rate of elements is used to define the target duration
        //
        virtual TimeDuration getTargetDuration();

        //
        // calcThrottle 
        //
        // Calculate the duration about that we should sleep based on processing time
        // based on value from getTargetDuration() minus processing time ( end time 
        // minus start time)
        //
        // If the value is a positive duration then the boost::this_thread::sleep
        // method is called with 1/4 of the calculated duration.
        //
        virtual TimeDuration calcThrottle( TimeMark &stime,
                                           TimeMark &etime );

        //
        // createBlock
        //
        // Create the actual GNU Radio Block to that will perform the work method. The resulting
        // block object is assigned to gr_sptr
        //
        // Add property change callbacks for getter/setter methods
        //
        //
        virtual void createBlock() = 0;

/*{% if hasOutput or hasInput %}*/
        // 
        // setupIOMappings
        //
        // Sets up mappings for input and output ports and GnuRadio Stream indexes
        //
        // A Gnu Radio input or output streams will be created for each defined RedHawk port.
        // The streams will be ordered 0..N-1 as they are defined in inputPortOrder and outputPortOrder
        // lists created during Component initialization.  
        //
        // For Gnu Radio blocks that define -1 for maximum number of input streams. The number of
        // input streams created will be restricted to the number of RedHawk ports.
        //
        // RESOLVE - need to base mapping for -1 condition on "connections" and not streams
        // RESOLVE - need to add parameters to define expected modes for input ports.. i.e. real or complex and 
        //           not have to wait for SRI.
        //
        virtual void  setupIOMappings();

/*{% endif %}*/
/*{% if hasOutput %}*/
        //
        // getNOutputStreams
        //
        // Called by setupIOMappings when the number of Gnu Radio output streams == -1 (variable ) and number of 
        // Redhawk ports  == 1.
    
        // @return uint32_t : Number of output streams to build
        //
        virtual uint32_t  getNOutputStreams(); 
 
        //
        // createOutputSRI
        //
        // Called by setupIOMappings when an output mapping is defined. For each output mapping
        // defined, a call to createOutputSRI will be issued with the associated output index.
        // This default SRI and StreamID will be saved to the mapping and pushed down stream via pushSRI.
        // The subclass is responsible for overriding behavior of this method. The index provide matches
        // the stream index number that will be use by the Gnu Radio  Block object
        //
        // @param idx : output stream index number to associate the returned SRI object with
        // @return sri : default SRI object passed down stream over a RedHawk port
        //      
        virtual BULKIO::StreamSRI  createOutputSRI( int32_t oidx, int32_t &in_idx, std::string &ext );

        virtual BULKIO::StreamSRI  createOutputSRI( int32_t oidx);

        //
        // adjustOutputRate
        //
        // Called by seOutputStreamSRI method when pushing SRI down stream to adjust the 
        // the xdelta and/or ydelta values accordingly.  The provided method will perform the following:
        //
        //  gr_blocks, gr_sync_block - no modifications are performed
        //  gr_sync_decimator - sri.xdelta * gr_sptr->decimation()
        //  gr_sync_interpolator - sri.xdelta  / gr_sptr->interpolate()
        //
        virtual void  adjustOutputRate(BULKIO::StreamSRI &sri );

/*{% endif %}*/
/*{% if hasInput %}*/
/*{% for port in component.ports if port['direction'] == 'provides' %}*/
        // callback when a new Stream ID is detected on the port so we can add to istream/ostream mapping list
        void  ${port.cppname}_newStreamCallback( BULKIO::StreamSRI &sri );

/*{% endfor %}*/
        void  processStreamIdChanges();

/*{% endif %}*/
/*{% if hasOutput %}*/
        //
        // setOutputSteamSRI
        //
        // Set the SRI context for an output stream from a Gnu Radio Block, when a pushPacket call occurs. Whenever the SRI is established
        // for an output stream it is sent down stream to the next component.
        //  
        virtual void  setOutputStreamSRI( int streamIdx, BULKIO::StreamSRI &in_sri, bool sendSRI=true, bool setStreamID=true ) {
            for (int i = 0; i < (int)io_mapping[streamIdx].size(); i++){
                int o_idx = io_mapping[streamIdx][i];
/*{% if  hasMultiOut %}*/
                _ostreams[o_idx]->adjustSRI(in_sri, o_idx, stream_id_map, outPorts, setStreamID, naming_service_name );
/*{% else %}*/
                _ostreams[o_idx]->adjustSRI(in_sri, o_idx, setStreamID );
/*{% endif %}*/
                if ( sendSRI ) _ostreams[o_idx]->pushSRI();
            }
        }

        //
        // setOutputSteamSRI
        //
        // Use the same SRI context for all output streams from a Gnu Radio Block, when a pushPacket call occurs. Whenever the SRI is established
        // for an output stream it is sent down stream to the next component.
        // 
        virtual void  setOutputStreamSRI( BULKIO::StreamSRI &in_sri , bool sendSRI = true, bool setStreamID = true ) {
            OStreamList::iterator ostream=_ostreams.begin();
            for( int o_idx=0;  ostream != _ostreams.end(); o_idx++, ostream++ ) {
/*{% if hasMultiOut %}*/
                (*ostream)->adjustSRI(in_sri, o_idx, stream_id_map, outPorts, setStreamID, naming_service_name );
/*{% else %}*/
                (*ostream)->adjustSRI(in_sri, o_idx, setStreamID );
/*{% endif %}*/
                if ( sendSRI )  (*ostream)->pushSRI();
            }
        }

/*{% endif %}*/
/*# COMMENT
  //
  // gr_istream
  //
  // RESOLVE: This class should to be integrated with the Port objects for a component
  // that would resolve a lot of the integration issues.. We already have a queue (vertically)
  // that holds all the incoming data... this class is just partitioning the data
  // queue (horizontally) based on stream id..  Too much data movement, re-copying of 
  // vector contents and then presenting this to the work method of the Gnu Radio Block.
  //
  // Need to look at strategies of limit the number of copies and the vector resizing
  // that happens.
  //
  // We also ran across the issue of data synchronization from multiple ports,
  // and a GR_SYNC blocks. There is always the possiblity of getting swapped out by the
  // scheduled at any time your process is running.
  //
  //
#*/
/*{% if hasInput %}*/
    //
    // gr_istream - Mapping of Provides Ports to Gnu Radio Stream indexes
    //
    // Gnu Radio Block input stream definition:
    //  Input = 1 .. N then each Provides Port type of X is mapped to a stream index 0..N-1
    //                  This assumes the component will only have 1 input port type. (i.e float ports)
    //  Input = -1  and single Provides Port interface then each unique stream definition will map to a stream index 0..N
    //  Input = -1  and N Provides Port interface then each port will map to a stream index 0..N-1
    //
    // The mapping items are stored in a vector and maintain by setIOMappings and notifySRI methods, and
    // the service function when "end of stream" happens.
    //
    struct gr_istream_base {
        GNU_RADIO_BLOCK_PTR                grb;              // shared pointer to our gr_block
        int                                _idx;             // index of stream in gr_block
        std::string                        streamID;         // redhawk stream id
        int                                _spe;             // scalars per element
        int                                _vlen;            // vector length in items, the gr_block process data 
        int                                _hlen;            // history length in items, the gr_blocks expects
        bool                               _eos;             // if EOS was received from port
        bool                               _sri;             // that we received an SRI call

        // Functions for child to implement
        virtual int SizeOfElement( int mode) = 0;
        virtual uint64_t nelems () = 0;
        virtual int read( int64_t ritems=-1 ) = 0;
        virtual bool overrun() = 0;
        virtual bool sriChanged() = 0;
        virtual void *read_pointer( int32_t items ) = 0;
        virtual void consume( int32_t n_items ) = 0;
        virtual void consume_elements( int32_t inNelems ) = 0;
        virtual void close() = 0;

        virtual void resizeData(int newSize) = 0;
        virtual void * getPort() = 0;
        virtual std::string getPktStreamId() = 0;
        virtual BULKIO::StreamSRI& getPktSri() = 0;
        virtual bool pktNull() = 0;
        virtual TimeStamp getPktTimeStamp() = 0;

        gr_istream_base( GNU_RADIO_BLOCK_PTR in_grb, int idx, int mode, std::string &sid ) :
        grb(in_grb), _idx(idx), streamID(sid), _spe(1), _vlen(1), _hlen(1), _eos(false), _sri(true)
        {
        };

        gr_istream_base( GNU_RADIO_BLOCK_PTR in_grb, int idx,  std::string &sid ) :
        grb(in_grb), _idx(idx), streamID(sid), _spe(1), _vlen(1), _hlen(1), _eos(false), _sri(false)
        {
        };

        //
        // translate scalars per element for incoming data
        //    mode == 0 : real, mode == 1 : complex
        static inline int ScalarsPerElement( int mode ) {
            int spe=1;
            if ( mode == 1 ) spe=2;
            return spe;
        };

        //
        // translate scalars per element for incoming data
        //    mode == 0 : real, mode == 1 : complex
        static inline int ScalarsPerElement( BULKIO::StreamSRI &sri ) {
            return ScalarsPerElement( sri.mode );
        };

        //
        // return scalars per element
        //
        inline int spe () {
            return _spe;
        }

        //
        // set scalars per element
        //
        inline int spe( int mode ) {
            _check( mode );
            return _spe;
        }

        //
        // return state if SRI was set
        //
        inline  bool sri() {
            return _sri;
        }

        inline  bool sri( bool newSri ) {
            _sri = newSri;
            return _sri;
        }

        //
        // return if End of Stream was seen
        //
        inline  bool eos() {
            return _eos;
        }

        inline  bool eos( bool newEos ) {
            _eos = newEos;
            return _eos;
        }

        inline int vlen () {
            return _vlen;
        }

        void _check( int inMode , bool force=false) {
            // calc old history value
            int32_t  old_hlen = (_hlen-1) * (_vlen*_spe);
            int32_t  spe=ScalarsPerElement(inMode);
            int32_t  nvlen=_vlen;
            bool     newVlen=false;
            bool     newSpe=false;
            try {
                if ( grb && grb->input_signature() )
                    nvlen = grb->input_signature()->sizeof_stream_item(_idx) / SizeOfElement(inMode);
            } catch(...) {
                LOG_TRACE( ${className}, "UNABLE TO SET VLEN, BAD INDEX:" << _idx );
            }

            if ( nvlen != _vlen && nvlen >= 1 ) {
                _vlen=nvlen;
                newVlen=true;
            }

            if ( spe != _spe ) {
                _spe = spe;
                newSpe = true;
            }

            if ( force || newSpe || newVlen ) {
                // seed history for buffer with empty items
                int32_t new_hlen = ( grb->history()-1)* ( _vlen * _spe );
                if ( (old_hlen != new_hlen)  && ( new_hlen > -1 ) ) {
                    _hlen = grb->history();
                    resizeData( new_hlen );
                }
            }
        }

        //
        // reset our association to a GR Block
        //    
        void associate( GNU_RADIO_BLOCK_PTR newBlock ) {
            grb = newBlock;
            if ( grb ) _check( _spe, true );
        }

        //
        //
        //
        inline uint64_t nitems () {
            uint64_t tmp = nelems();
            if ( _vlen > 0 ) tmp /= _vlen;
            return tmp;
        }

        uint64_t  itemsToScalars( uint64_t N ) {
            return  N*_vlen*_spe;
        };
    };

    template < typename IN_PORT_TYPE > struct gr_istream : gr_istream_base {
        IN_PORT_TYPE                       *port;            // RH port object
/*{% if mem_align %}*/
        std::vector< typename IN_PORT_TYPE::NativeType, GR_MemAlign< typename IN_PORT_TYPE::NativeType > >      _data;     // buffered data from port
/*{% else %}*/        
        std::vector< typename IN_PORT_TYPE::NativeType >      _data;     // buffered data from port
/*{% endif %}*/
        typename IN_PORT_TYPE::dataTransfer *pkt;            // pointer to last packet read from port

        gr_istream( IN_PORT_TYPE *in_port, GNU_RADIO_BLOCK_PTR in_grb, int idx, int mode, std::string &sid ) :
        gr_istream_base(in_grb, idx, mode, sid), port(in_port), _data(0), pkt(NULL)
        {
            _spe = ScalarsPerElement(mode);
            _check(mode, true);
        };

        gr_istream( IN_PORT_TYPE *in_port, GNU_RADIO_BLOCK_PTR in_grb, int idx,  std::string &sid ) :
        gr_istream_base(in_grb, idx, sid), port(in_port), _data(0), pkt(NULL)
        {
            int mode=0;
            _spe = ScalarsPerElement(mode);
            _check(mode, true);
        };

        //
        // Return the size of an element (sample) in bytes
        //
        inline int SizeOfElement(int mode ) {
            return sizeof( typename IN_PORT_TYPE::NativeType)*ScalarsPerElement( mode);
        };

        //
        // Return the size of an element (sample) in bytes
        //
        static inline int SizeOfElement( BULKIO::StreamSRI &sri ) {
            return sizeof( typename IN_PORT_TYPE::NativeType)*ScalarsPerElement(sri);
        }

        inline uint64_t nelems () {
            uint64_t tmp = _data.size();
            if ( _spe > 0 ) tmp /= _spe;
            return tmp;
        }

        // RESOLVE: need to allow for requests of certain size, and blocking and timeouts
        int   read( int64_t ritems=-1 ) {

            int retval = -1;
            typename IN_PORT_TYPE::dataTransfer *tpkt;

            if ( port && _sri ) {
                tpkt = port->getPacket( -1, streamID );

                if ( tpkt == NULL ) {
                    if ( port != NULL && port->blocked() )  retval = 0;
               } else {
                    _data.insert( _data.end(), tpkt->dataBuffer.begin(), tpkt->dataBuffer.end() );
                    if ( tpkt->sriChanged ) {
                        spe(tpkt->SRI.mode);
                    }

                    // resolve need to keep time stamp accurate for first sample of data.... we could loose this if we
                    // end having residual data left in the buffer when output_multiple and vlen are used
                    // by the gr_block - read and consume_elements need refactoring

                    _eos = tpkt->EOS;
                    if ( pkt !=  NULL )  delete pkt;
                    pkt = tpkt;
                    retval=nitems();
                }    
            }

            return retval;
        }

        inline bool overrun() {
            return ( pkt && pkt->inputQueueFlushed);
        }

        inline bool sriChanged() {
            return ( pkt && pkt->sriChanged );
        }

        inline std::string getPktStreamId() {
            return pkt->streamID;
        }

        inline BULKIO::StreamSRI& getPktSri() {
            return pkt->SRI;
        }

        inline bool pktNull() {
            return pkt == NULL;
        }

        inline TimeStamp getPktTimeStamp() {
            return pkt->T;
        }

        void *read_pointer( int32_t items ) {
            uint32_t idx = itemsToScalars(items);
            if ( idx < _data.size() ) 
                return (void*) &_data[ idx ];
            else
                return (void*) &_data[0];
        }

        // compress data buffer for requested number of items
        void consume( int32_t n_items ) {
            if ( n_items > 0 ) {
                consume_elements( n_items*_vlen );
            }
        }

        // compress data buffer for requested number of items
        void consume_elements( int32_t inNelems ) {
             int d_idx = inNelems*_spe;
             int n = std::distance( _data.begin() + d_idx, _data.end() );
             if ( d_idx > 0 && n >= 0  ) {
                  std::copy( _data.begin() + d_idx, _data.end(), _data.begin() );
                  _data.resize(n);
              }
        }

        // perform clean up of stream state and mapping
        void close() {
            _data.clear();
            _vlen = 1;
            _hlen=1;
            _eos = false;
            _sri = false;
            if ( pkt ) {
                delete pkt;
                pkt=NULL;
            }
        }

        void resizeData(int new_hlen) {
            _data.resize( new_hlen );
        }

        void * getPort(){
            return (void*) port;
        }
    };

/*{% endif %}*/
/*{% if hasOutput %}*/
    // gr_ostream
    //
    // Provides a mapping of output ports to a Gnu Radio  Block's output stream.  These items
    // are stored in a vector for managing output from the Gnu Radio Block and pushing
    // the data down stream to the next component over the port object.
    //
    // Items in the vector are maintain by setIOMappings, notifySRI and the
    // the service function when "end of stream" happens
    //
    struct gr_ostream_base {
        GNU_RADIO_BLOCK_PTR                grb;                  // shared pointer ot GR_BLOCK
        int                                _idx;                 // output index (loose association)
        std::string                        _ext;                 // extension to append to incoming StreamID
        std::string                        streamID;             // Stream Id to send down stream
        BULKIO::StreamSRI                  sri;                  // SRI to send down stream
        bool                               _m_tstamp;            // set to true if we are maintaining outgoing time stamp
        BULKIO::PrecisionUTCTime           tstamp;               // time stamp to use for pushPacket calls
        bool                               _eos;                        // if EOS was sent
        uint64_t                           _nelems;                     // number of elements in that have been pushed down stream
        int                                _vlen;                      // vector length in items, to allocate output buffer for GR_BLOCK

        // Functions for child to implement
        virtual int SizeOfElement( int mode) = 0;
        virtual void pushSRI() = 0;
        virtual void pushSRI( BULKIO::StreamSRI &inSri ) = 0;
        virtual uint64_t  nelems() = 0;
        virtual void resize( int32_t n_items ) = 0;
        virtual void *write_pointer() = 0;
        virtual int  write( int32_t n_items, bool eos, TimeStamp &ts, bool adjust_ts=false ) = 0;
        virtual int  write( int32_t n_items, bool eos, bool adjust_ts ) = 0;
        virtual int  write( int32_t n_items, bool eos ) = 0;
        virtual void close() = 0;

        gr_ostream_base( GNU_RADIO_BLOCK_PTR ingrb, int idx, int mode, std::string &in_sid, const std::string &ext="" ) :
            grb(ingrb), _idx(idx), _ext(ext), streamID(in_sid), _m_tstamp(false), _eos(false), _nelems(0), _vlen(1)
        {
            sri.hversion = 1;
            sri.xstart = 0.0;
            sri.xdelta = 1;
            sri.xunits = BULKIO::UNITS_TIME;
            sri.subsize = 0;
            sri.ystart = 0.0;
            sri.ydelta = 0.0;
            sri.yunits = BULKIO::UNITS_NONE;
            sri.mode = mode;
            sri.streamID = streamID.c_str();
            // RESOLVE sri.blocking=0;   to block or not

            tstamp.tcmode = BULKIO::TCM_CPU;
            tstamp.tcstatus = (short)1;
            tstamp.toff = 0.0;
            setTimeStamp();
        }

        //
        // translate scalars per element for incoming data
        //    mode == 0 : real, mode == 1 : complex
        static inline int ScalarsPerElement( int mode ) {
            int spe=1;
            if ( mode == 1 ) spe=2;
            return spe;
        };
    
        //
        // translate scalars per element for incoming data
        //    mode == 0 : real, mode == 1 : complex
        static inline int ScalarsPerElement( BULKIO::StreamSRI &sri ) {
            return ScalarsPerElement( sri.mode );
        };

        //
        // Establish and SRI context for this output stream
        //
        void setSRI( BULKIO::StreamSRI &inSri, int idx ) {
            sri=inSri;
            streamID = sri.streamID;
            // check if history, spe and vlen need to be adjusted
            _check(idx);
        };

        //
        // Only adjust stream id and output rate for SRI object
        //
/*{% if hasMultiOut %}*/
        void adjustSRI( BULKIO::StreamSRI &inSri, int idx, const std::vector<stream_id_struct_struct> &stream_id_map, PortSupplier_impl::RH_UsesPortMap& outPorts, bool setStreamID=true, const std::string &stream_tag="" ) {
            if ( setStreamID ) {
                std::vector<std::string> outPortNames;
                for (RH_UsesPortMap::iterator port = outPorts.begin(); port != outPorts.end(); ++port ) {
                    outPortNames.push_back(port->first);
                }
                std::string s(inSri.streamID);
                std::ostringstream t;
                if (stream_id_map.size() != 0) {
                    for(int i = 0; i < (int)stream_id_map.size(); i++) {
                        if (outPortNames[idx] == stream_id_map[i].port_name) {
                            std::string stream = stream_id_map[i].stream_id;
                            t << inSri.streamID << "_";
                            stream = boost::replace_all_copy( stream, "%SID", t.str() );
                            stream = boost::replace_all_copy( stream, "%C", stream_tag + "_" );
                            t.str("");
                            t << idx << "_";
                            stream = boost::replace_all_copy( stream, "%D", t.str() );
                            streamID = stream;
                            sri.streamID = stream.c_str();
                         }
                    }
                    outPortNames.clear();
                }
                else {
                    t << s << _ext;
                    streamID = t.str();
                    sri.streamID = t.str().c_str();
                }
            }
/*{%else %}*/
        void adjustSRI( BULKIO::StreamSRI &inSri, int idx, bool setStreamID=true ) {
            if ( setStreamID ) {
                std::string s(inSri.streamID);
                std::ostringstream t;
                t << s << _ext;
                streamID = t.str();
                sri.streamID = t.str().c_str();
            }
/*{% endif %}*/
            double ret=inSri.xdelta;
            if ( grb ) ret = ret *grb->relative_rate();
            sri.xdelta = ret;
            _check(idx);
        };

        //
        // Set our stream ID ...
        //
        void setStreamID( std::string &sid ) {
            streamID=sid;
        };

        //
        // Return the number of scalars per element (sample) that we use
        //
        inline int spe() {
            return ScalarsPerElement(sri.mode);
        }

        //
        // return the state if EOS was pushed down stream
        //
        inline bool eos () {
            return _eos;
        }

        //
        // return the vector length to process data by the GR_BLOCK
        //
        inline int vlen() {
            return _vlen;
        }

        inline bool eos ( bool inEos ) {
            _eos=inEos;
            return _eos;
        }

        void _check( int idx ) {
            if ( grb ) {
                int nvlen=1;
                try {
                    if ( grb && grb->output_signature() )
                    nvlen = grb->output_signature()->sizeof_stream_item(idx) / SizeOfElement(sri.mode);
                    if ( nvlen != _vlen && nvlen >= 1 ) _vlen=nvlen;
                } catch(...) {
                    LOG_TRACE( ${className}, "UNABLE TO SET VLEN, BAD INDEX:" << _idx );
                }
            }
        }
    
        //
        // establish and assocation with a new GR_BLOCK
        //
        void associate( GNU_RADIO_BLOCK_PTR newblock ) {
            grb = newblock;
            _check( _idx );
        }

        //
        // return the number of items in the output buffer
        //
        inline uint64_t nitems () {
            uint64_t tmp=nelems();
            if ( _vlen > 0 ) tmp /= _vlen;
            return tmp;
        }

        //
        // return the number of scalars used for N number of items
        //
        inline uint64_t itemsToScalars( uint64_t N ) {
            return  N*_vlen*spe();
        };

        //
        // return the number of output elements sent down stream
        //
        inline uint64_t  oelems() {
            return _nelems;
        };

        //
        // return the number of output items sent down stream
        //
        inline uint64_t  oitems() {
            uint64_t tmp = _nelems;
            if ( _vlen > 0 ) tmp /= _vlen;
            return tmp;
        };

        //
        // Turn time stamp calculations on or off
        //
        void setAutoAdjustTime( bool onoff ) {
            _m_tstamp = onoff;
        };

        //
        // sets time stamp value to be time of day
        //
        void setTimeStamp( ) {
            struct timeval tmp_time;
            struct timezone tmp_tz;
            gettimeofday(&tmp_time, &tmp_tz);
            tstamp.twsec = tmp_time.tv_sec;
            tstamp.tfsec = tmp_time.tv_usec / 1e6;
        };

        //
        // set time stamp value for the stream to a specific value, turns on
        // stream's monitoring of time stamp
        //
        void setTimeStamp( TimeStamp &inTimeStamp, bool adjust_ts=true ) {
            _m_tstamp = adjust_ts;
            tstamp = inTimeStamp;
        };

        void forwardTimeStamp( int32_t noutput_items, TimeStamp &ts ) {
            double twsec = ts.twsec;
            double tfsec = ts.tfsec;
            double sdelta=sri.xdelta;
            sdelta  = sdelta * noutput_items*_vlen;
            double new_time = (twsec+tfsec)+sdelta;
            ts.tfsec = std::modf( new_time, &ts.twsec );
        };

        void forwardTimeStamp( int32_t noutput_items ) {
            double twsec = tstamp.twsec;
            double tfsec = tstamp.tfsec;
            double sdelta=sri.xdelta;
            sdelta  = sdelta * noutput_items*_vlen;
            double new_time = (twsec+tfsec)+sdelta;
            tstamp.tfsec = std::modf( new_time, &tstamp.twsec );
        };
    };

    template < typename OUT_PORT_TYPE > struct gr_ostream : gr_ostream_base {
        OUT_PORT_TYPE                      *port;                // handle to Port object
        std::string                        _ext;                 // extension to append to incoming StreamID
/*{% if mem_align %}*/        
        std::vector< typename OUT_PORT_TYPE::NativeType, GR_MemAlign< typename OUT_PORT_TYPE::NativeType > >  _data;    // output buffer used by GR_Block
/*{% else %}*/
        std::vector< typename OUT_PORT_TYPE::NativeType >  _data;    // output buffer used by GR_Block
/*{% endif %}*/
    
        gr_ostream( OUT_PORT_TYPE *out_port, GNU_RADIO_BLOCK_PTR ingrb, int idx, int mode, std::string &in_sid, std::string &ext="" ) :
            gr_ostream_base(ingrb, idx, mode, in_sid, ext), port(out_port), _ext(ext),_data(0)
        {
        };
    
        //
        // Return the size of an element (sample) in bytes
        //
        inline int SizeOfElement(int mode ) {
            return sizeof( typename OUT_PORT_TYPE::NativeType)*ScalarsPerElement( mode);
        };
    
        //
        // Return the size of an element (sample) in bytes
        //
        static inline int SizeOfElement( BULKIO::StreamSRI &sri ) {
            return sizeof( typename OUT_PORT_TYPE::NativeType)*ScalarsPerElement(sri);
        };
    
        //
        // push our SRI object down stream
        //
        void pushSRI() {
            if ( port ) port->pushSRI( sri );
        };
    
        //
        // push incoming SRI object down stream, do not save this object
        //
        void pushSRI( BULKIO::StreamSRI &inSri ) {
            if ( port ) port->pushSRI( inSri );
        };
    
        //
        // return the number of elements (samples) in the output buffer
        //
        inline uint64_t  nelems() {
            uint64_t tmp = _data.size();
            if ( spe() > 0 ) tmp /= spe();
            return tmp;
        };
   
        //
        // resize the output buffer to N number of items
        //
        void resize( int32_t n_items ) {
            if ( _data.size() != (size_t)(n_items*spe()*_vlen) ) {
                _data.resize( n_items*spe()*_vlen );
            }
        }
    
        void *write_pointer(){
            // push ostream's buffer address onto list of output buffers
            return (void*) &(_data[0]);
        }
    
        //
        // write data to output ports using the provided time stamp and adjust the time
        // accordingly using the xdelta value of the SRI and the number of items
        //
        int  write( int32_t n_items, bool eos, TimeStamp &ts, bool adjust_ts=false ) {
    
            resize( n_items );
/*{% if mem_align %}*/ 
            typename OUT_PORT_TYPE::NativeSequenceType nst(_data.begin(),_data.end() );
            if ( port ) port->pushPacket( nst, ts, eos, streamID );
/*{% else %}*/
            if ( port ) port->pushPacket( _data, ts, eos, streamID );
/*{% endif %}*/
    
            if ( adjust_ts ) forwardTimeStamp( n_items, ts );
    
            _eos = eos;
            _nelems += (n_items*_vlen);
            return n_items;
        };
    
        //
        // write data to the output port using the map object's timestamp
        // if the adjust_ts value equals true. otherwise use time of
        // day for the time stamp
        //
        int  write( int32_t n_items, bool eos, bool adjust_ts ) {
            if ( !adjust_ts ) setTimeStamp();
    
            resize( n_items );
/*{% if mem_align %}*/
            typename OUT_PORT_TYPE::NativeSequenceType nst(_data.begin(),_data.end() );
            if ( port ) port->pushPacket( nst, tstamp, eos, streamID );
/*{% else %}*/
            if ( port ) port->pushPacket( _data, tstamp, eos, streamID );
/*{% endif %}*/
            if ( adjust_ts ) forwardTimeStamp( n_items );
    
            _eos = eos;
            _nelems += (n_items*_vlen);
            return n_items;
        };
    
    
        //
        // write data to the output port using the map object's timestamp and
        // adjust the time stamp if the maps's m_tstamp value == true
        //
        int  write( int32_t n_items, bool eos ) {
            if ( !_m_tstamp ) setTimeStamp();
    
            resize( n_items );
/*{% if mem_align %}*/
            typename OUT_PORT_TYPE::NativeSequenceType nst(_data.begin(),_data.end() );
            if ( port ) port->pushPacket( nst, tstamp, eos, streamID );
/*{% else %}*/
            if ( port ) port->pushPacket( _data, tstamp, eos, streamID );
/*{% endif %}*/
            if ( _m_tstamp ) forwardTimeStamp( n_items );
    
            _eos = eos;
            _nelems += n_items*_vlen;
            return n_items;
        };
    
        // perform clean up on the stream state and map
        void close() {
            _data.clear();
            _vlen=1;
            _eos = false;
            _m_tstamp=false;
        };
    };

/*{% endif %}*/
    typedef  gr_vector_const_void_star                GR_IN_BUFFERS;
    typedef  gr_vector_void_star                      GR_OUT_BUFFERS;
    typedef  gr_vector_int                            GR_BUFFER_LENGTHS;

/*{% if hasInput %}*/
    int  _analyzerServiceFunction( std::vector< gr_istream_base * > &istreams );
    int  _forecastAndProcess( bool &eos, std::vector< gr_istream_base * > &istreams );

/*{% endif %}*/
/*{% if hasOutput %}*/
    int  _generatorServiceFunction( std::vector< gr_ostream_base * >  &ostreams );

/*{% endif %}*/
/*{% if hasInput and hasOutput %}*/
    int  _transformerServiceFunction( std::vector< gr_istream_base * > &istreams,
                                                                     std::vector< gr_ostream_base * > &ostreams ); 
    int  _forecastAndProcess( bool &eos, std::vector< gr_istream_base * > &istreams,
                                                                     std::vector< gr_ostream_base * > &ostreams ); 

/*{% endif %}*/
/*{% if hasInput %}*/
    typedef std::deque< std::pair< void*, BULKIO::StreamSRI > >  SRIQueue;
    typedef std::vector< gr_istream_base * >    IStreamList;

/*{% endif %}*/
/*{% if hasOutput %}*/
    typedef std::vector< gr_ostream_base * >   OStreamList;

/*{% endif %}*/

    // cache variables to transferring data to/from a GNU Radio Block
    std::vector<bool>            _input_ready;
    GR_BUFFER_LENGTHS            _ninput_items_required;
    GR_BUFFER_LENGTHS            _ninput_items;
    GR_IN_BUFFERS                _input_items;
    GR_OUT_BUFFERS               _output_items;
    int32_t                      noutput_items;

/*{% if hasInput %}*/
    boost::mutex                _sriMutex;  
    SRIQueue                    _sriQueue;

/*{% endif %}*/
    // mapping of RH ports to GNU Radio streams
/*{% if hasInput %}*/
    IStreamList                  _istreams;

/*{% endif %}*/
/*{% if hasOutput %}*/
    OStreamList                  _ostreams;
    bool                         sentEOS;

/*{% endif %}*/

    ENABLE_LOGGING;
    
    protected:

        bool                     _maintainTimeStamp;
        bool                     _throttle;
        TimeMark                 p_start_time;
        TimeMark                 p_end_time;

    public:

        int serviceFunction()
        {
            int retval = NOOP;
/*{% if hasInput and not hasOutput %}*/
            retval = _analyzerServiceFunction( _istreams );
/*{% elif not hasInput and hasOutput %}*/
            retval = _generatorServiceFunction( _ostreams );
/*{% elif hasInput and hasOutput %}*/
            retval = _transformerServiceFunction( _istreams, _ostreams );
/*{% endif %}*/

            p_end_time =  boost::posix_time::microsec_clock::local_time();
            if ( retval == NORMAL && _throttle ) {
                TimeDuration  delta = calcThrottle( p_start_time, p_end_time );
                if ( delta.is_not_a_date_time() == false && delta.is_negative() == false ) {
                    LOG_TRACE( ${className}, " SLEEP ...." << delta );
                    boost::this_thread::sleep( delta );
                } else {
                    LOG_TRACE( ${className}, " NO SLEEPING...." );
                }
            }
            p_start_time = p_end_time;
       
            LOG_TRACE( ${className}, " serviceFunction: retval:" << retval);

            return retval;
        };
/*{% endblock %}*/
