
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
#if (!__MonoCS__)
using System.Speech.Synthesis;
#endif
using System.Xml;
using System.IO;
using System.Threading;
using System.Runtime.InteropServices;


namespace TtsRelay
{
    public static class IntPtrExtensions
    {
        public static IntPtr Increment(this IntPtr ptr, int cbSize)
        {
            return new IntPtr(ptr.ToInt64() + cbSize);
        }

        public static IntPtr Increment<T>(this IntPtr ptr)
        {
            return Increment(ptr, Marshal.SizeOf(typeof(T)));
        }

        public static T ElementAt<T>(this IntPtr ptr, int index)
        {
            var offset = Marshal.SizeOf(typeof(T)) * index;
            var offsetPtr = ptr.Increment(offset);
            return (T)Marshal.PtrToStructure(offsetPtr, typeof(T));
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct GenerateAudioReplyInterop
    {
        public IntPtr soundFile;
        public uint workBreakListNum;
        public IntPtr wordBreakListStart;
        public IntPtr wordBreakListEnd;
        public uint markListNum;
        public IntPtr markListName;
        public IntPtr markListTime;
        public uint visemeListNum;
        public IntPtr visemeListType;
        public IntPtr visemeListStart;
        public IntPtr visemeListArticulation;
    }


    public class RvoiceDLL
    {
        public delegate void Callback( IntPtr pContext, IntPtr event_type, IntPtr data_chunk, IntPtr data_type, IntPtr data_length, IntPtr phone_set, IntPtr cparam1, IntPtr cparam2 );

        [DllImport("rhetrapi.dll")]
        public static extern int rAPI_Version_Major();

        [DllImport("rhetrapi.dll")]
        public static extern int rAPI_Version_Minor();

        [DllImport("rhetrapi.dll")]
        public static extern int rAPI_Create_Handle(
            /* Need phone and word events       */  int events_requested,
            /* Data type required               */  int data_type,
            /* User callback function           */  Callback function,
            /* User data                        */  IntPtr user_data);

        [DllImport("rhetrapi.dll")]
        public static extern int rAPI_Close_Handle(int handle);

        [DllImport("rhetrapi.dll")]
        public static extern int rAPI_Speak(int handle, [MarshalAs(UnmanagedType.LPStr)]string voice, [MarshalAs(UnmanagedType.LPStr)]string text);

        [DllImport("rhetrapi.dll")]
        public static extern int rAPI_Set_Servers([MarshalAs(UnmanagedType.LPStr)]string server_list);

        [DllImport("rhetrapi.dll")]
        public static extern int rAPI_Use_Server(int handle, [MarshalAs(UnmanagedType.LPStr)]string server_name);

        [DllImport("rhetrapi.dll")]
        public static extern void rAPI_Clean_Up();

        [DllImport("rhetrapi.dll")]
        public static extern int rAPI_Enumerate_Server_Voices(int handle, string server);

        [DllImport("rhetrapi.dll")]
        public static extern string rAPI_Next_Voice(int handle);

        [DllImport("rhetrapi.dll")]
        public static extern int rAPI_Set_XML(int handle, int xml_type);

        public static int RAPI_ALL_EVENTS=0x01;
        public static int RAPI_PCM_16bit_22050Hz=0x0F;


        [DllImport("RvoiceDLL.dll")]
        public static extern bool RVOICE_DLL_Init([MarshalAs(UnmanagedType.LPStr)] string visemeMapping);

        [DllImport("RvoiceDLL.dll")]
        public static extern void RVOICE_DLL_SetVisemeMapping([MarshalAs(UnmanagedType.LPStr)] string visemeMapping);

        [DllImport("RvoiceDLL.dll")]
        public static extern bool RVOICE_DLL_GenerateAudio([MarshalAs(UnmanagedType.LPStr)] string message, [MarshalAs(UnmanagedType.LPStr)] string outputFilename, [MarshalAs(UnmanagedType.LPStr)] string messageOutputFileName, [MarshalAs(UnmanagedType.LPStr)] string voice, [Out] StringBuilder xmlReplyReturn, ref GenerateAudioReplyInterop generateAudioReplyInterop);
    }

    public class RvoiceRelay : ITtsRelay
    {
        public int m_rvoiceHandle;

        // public delegate void Callback( int characterID, string characterType, string characterName, int skeletonType, IntPtr userData);
#if false
        static void RapiCallbackFunction(
                                void       *pContext,
                                long        event_type,
                                const char *data_chunk,
                                long        data_type,
                                long        data_length,
                                const char *phone_set,
                                const char *cparam1,
                                const char *cparam2
                                );
#endif


        public bool Init(string visemeMapping)
        {
            RvoiceDLL.RVOICE_DLL_Init(visemeMapping);

#if false
            Console.WriteLine( "Rvoice version: " + RvoiceDLL.rAPI_Version_Major() );
            Console.WriteLine( "Rvoice version: " + RvoiceDLL.rAPI_Version_Minor() );

            RvoiceDLL.rAPI_Set_Servers( "bootcamp.ict.usc.edu" );
            m_rvoiceHandle = RvoiceDLL.rAPI_Create_Handle( RvoiceDLL.RAPI_ALL_EVENTS, RvoiceDLL.RAPI_PCM_16bit_22050Hz, new RvoiceDLL.Callback( OnCallback ), new IntPtr(0) );
            RvoiceDLL.rAPI_Use_Server( m_rvoiceHandle, "bootcamp.ict.usc.edu" );
            RvoiceDLL.rAPI_Set_XML( m_rvoiceHandle, true ? 1 : 0 );

            //////////

            // result=rapi.RegisterCallback( RAPI_ALL_EVENTS, RAPI_PCM_16bit_22050Hz, this );
            // rapi.setXML( true );
#endif

            return true;
        }


        public string [] GetVoices()
        {
            return new string[] { "M021", "M009" };
        }


        public void SetVisemeMapping(string visemeMapping)
        {
            RvoiceDLL.RVOICE_DLL_SetVisemeMapping(visemeMapping);
        }


        public bool GenerateAudio(string message, string outputFileName, string messageOutputFileName, string voice, ref string xmlReplyReturn, ref GenerateAudioReply generateAudioReplyReturn)
        {
            StringBuilder reply = new StringBuilder(65536);
            GenerateAudioReplyInterop generateAudioReplyInterop = new GenerateAudioReplyInterop();
            RvoiceDLL.RVOICE_DLL_GenerateAudio( message, outputFileName, messageOutputFileName, voice, reply, ref generateAudioReplyInterop );

            xmlReplyReturn = "";

            generateAudioReplyReturn.used = true;
            generateAudioReplyReturn.soundFile = Marshal.PtrToStringAnsi(generateAudioReplyInterop.soundFile);
            generateAudioReplyReturn.WordBreakList = new List<KeyValuePairS<double,double>>();
            generateAudioReplyReturn.MarkList = new List<KeyValuePairS<string,double>>();
            generateAudioReplyReturn.VisemeList = new List<GenerateAudioReplyViseme>();

            for (int i = 0; i < generateAudioReplyInterop.workBreakListNum; i++)
            {
                double wordBreakListStart = generateAudioReplyInterop.wordBreakListStart.ElementAt<double>(i);
                double wordBreakListEnd = generateAudioReplyInterop.wordBreakListEnd.ElementAt<double>(i);
                generateAudioReplyReturn.WordBreakList.Add(new KeyValuePairS<double, double>(wordBreakListStart, wordBreakListEnd));
            }

            for (int i = 0; i < generateAudioReplyInterop.markListNum; i++)
            {
                string markListName = Marshal.PtrToStringAnsi(generateAudioReplyInterop.markListName.ElementAt<IntPtr>(i));
                double markListTime = generateAudioReplyInterop.markListTime.ElementAt<double>(i);
                generateAudioReplyReturn.MarkList.Add(new KeyValuePairS<string, double>(markListName, markListTime));
            }

            for (int i = 0; i < generateAudioReplyInterop.visemeListNum; i++)
            {
                string visemeListType = Marshal.PtrToStringAnsi(generateAudioReplyInterop.visemeListType.ElementAt<IntPtr>(i));
                double visemeListStart = generateAudioReplyInterop.visemeListStart.ElementAt<double>(i);
                double visemeListArticulation = generateAudioReplyInterop.visemeListArticulation.ElementAt<double>(i);
                generateAudioReplyReturn.VisemeList.Add(new GenerateAudioReplyViseme(visemeListType, visemeListStart, visemeListArticulation));
            }

#if false
            Console.WriteLine( "GenerateAudio: {0} - {1} - {2} - {3}", message, outputFileName, messageOutputFileName, voice );

            int result = RvoiceDLL.rAPI_Speak( m_rvoiceHandle, voice, "" );
#endif

            return true;
        }


        public void OnCallback( IntPtr pContext, IntPtr event_type, IntPtr data_chunk, IntPtr data_type, IntPtr data_length, IntPtr phone_set, IntPtr cparam1, IntPtr cparam2 )
        {
            //Console.WriteLine( "OnCallback: {0} - {1} - {2}", event_type, data_type, data_length );
        }



#if false
    public void OnCallback( int nEventType, byte[] abDataChunk, int nDataType,
        String strPhoneSet, String strEventData1,
        String strEventData2 )
    {
        //System.out.println( "OnCallback( nEventType="+nEventType+", ... )" );

        //
        // Switch on the type of event that has occurred
        //
        try
        {
            switch( nEventType )
            {
                case 0x06: // RAPI_WORD_BOUNDARY
                    System.out.println( "---- @"+TIME_FORMAT.format(curTime)  );
                    wordBreaks.add( new Double( curTime ) );
                    if( outputXml != null &&
                        outputXml.getTagName().equals(TAG_WORD) )
                    {
                        outputXml.setAttribute( ATTR_END, Double.toString(curTime) );
                        outputXml= ( Element )outputXml.getParentNode();
                    }
                    if( curTime < firstWordBreak )
                        firstWordBreak = curTime;
                    lastWordBreak = curTime;
                    break;

                case 0x07: // RAPI_PHON_BOUNDARY
                    if( !strPhoneSet.equals( phoneSet ) )
                    {
                        phoneSet=strPhoneSet;
                        System.out.println();
                        System.out.println( "Phoneme set \""+phoneSet+"\": " );
                    }
                    String viseme=mapToViseme( phoneSet, strEventData1 );
                    System.out.print( viseme+" @"+TIME_FORMAT.format(curTime)+" " );
                    phonemes.add( new PhonemeTime( viseme, curTime ) );

                    if( outputXml != null )
                    {
                        String curTimeStr = Double.toString( curTime );

                        Document xmlDoc = outputXml.getOwnerDocument();
                        Element visemeElem = xmlDoc.createElement( TAG_VISEME );
                        visemeElem.setAttribute( ATTR_TYPE, viseme );
                        visemeElem.setAttribute( ATTR_START, curTimeStr );

                        boolean isInWord = outputXml.getTagName().equals(TAG_WORD);
                        if( isInWord )
                        {  // continuing word
                            outputXml.appendChild( xmlDoc.createTextNode("   " ) );
                        }
                        else
                        {
                            outputXml.appendChild( xmlDoc.createTextNode( "\n   " ) );
                            if( !viseme.equals( "_" ) )
                            { // new word
                                Element wordElem=xmlDoc.createElement( TAG_WORD );
                                wordElem.setAttribute( ATTR_START, curTimeStr );
                                outputXml.appendChild( wordElem );
                                outputXml=wordElem;
                                outputXml.appendChild( xmlDoc.createTextNode( "\n      " ) );
                                isInWord=true;

                                if( firstWord==null )
                                    firstWord = wordElem;
                                lastWord = wordElem;
                            }
                        }
                        outputXml.appendChild( visemeElem );
                        if( isInWord )
                            outputXml.appendChild( xmlDoc.createTextNode( "\n   " ) );
                    }
                    break;

                case 0x0C: // RAPI_BOOKMARK_EVENT
                    System.out.print( "<"+strEventData1+" @"+TIME_FORMAT.format(curTime)+"> " );

                    if( outputXml != null )
                    {
                        bookmarks.add( new BookmarkTime( strEventData1, curTime,
                            outputXml, outputXml.getLastChild() ) );

                        //  Old way of immediately adding to XML
                        //String curTimeStr = Double.toString( curTime );
                        //
                        //Document xmlDoc = outputXml.getOwnerDocument();
                        //Element markElem = xmlDoc.createElement( TAG_MARK );
                        //markElem.setAttribute( ATTR_NAME, strEventData1 );
                        //markElem.setAttribute( ATTR_TIME, Double.toString(curTime) );
                        //
                        //boolean isInWord = outputXml.getTagName().equals(TAG_WORD);
                        //if( isInWord ) {  // continuing word
                        //    outputXml.appendChild( xmlDoc.createTextNode("   " ) );
                        //    outputXml.appendChild( markElem );
                        //    outputXml.appendChild( xmlDoc.createTextNode( "\n   " ) );
                        //} else {
                        //    outputXml.appendChild( xmlDoc.createTextNode( "\n   " ) );
                        //    outputXml.appendChild( markElem );
                        //}
                    }
                    break;

                case 0x10: // RAPI_AUDIO_EVENT
                    if( abDataChunk == null )
                        return;  // occassionally happens, not sure why (Anm)
                    try
                    {
                        int length=abDataChunk.length;
                        waveSize+=length;
                        if( DATA_FORMAT == 44100 )
                        {
                            waveSize+=length;  // samples are doubled
                            curTime = ((double)waveSize)/88200;  // 88200 bytes per second (two bytes per frame)
                            if( outputAudio != null )
                            {
                                byte[] sample=new byte[2];
                                for( int i=0; i<length; i++ )
                                {
                                    // Rhetorical is Intel... write out network endian
                                    sample[1]=abDataChunk[i];
                                    sample[0]=abDataChunk[++i];
                                    // up sample from 22050 by double each sample
                                    outputAudio.write( sample );
                                    outputAudio.write( sample );
                                }
                            }
                        }
                        else
                        {
                            switch( DATA_FORMAT )
                            {
                                case RAPI_PCM_16bit_8KHz:
                                    curTime= ((double)waveSize)/16000;  // 16000 bytes per second (two bytes per frame)
                                    break;
                                case RAPI_PCM_16bit_16KHz:
                                    curTime= ((double)waveSize)/32000;  // 32000 bytes  per second (two bytes per frame)
                                    break;
                                case RAPI_PCM_16bit_22050Hz:
                                    curTime= ((double)waveSize)/44100;  // 44100 bytes per second (two bytes per frame)
                            }
                            if( outputAudio!=null )
                            {
                                byte[] sample=new byte[2];
                                for( int i=0; i<length; i+=2 )
                                {
                                    // Rhetorical is intel... write out network endian
                                    outputAudio.write( abDataChunk[i+1] );
                                    outputAudio.write( abDataChunk[i] );
                                }
                            }
                        }
                    }
                    catch( IOException error )
                    {
                        StringWriter sw=new StringWriter();
                        PrintWriter pw=new PrintWriter( sw );
                        error.printStackTrace( pw );
                        pw.flush();
                        err( "Failed to write audio data to \""+filename+"\"\n"+
                            sw.toString() );

                        // data corrupt, destroy rest...    :P
                        try
                        {
                            outputAudio.close();
                        }
                        catch( IOException error2 ) {} // ignore, errored already
                        outputAudio=null;
                    }
                    break;

                case 0x0E: // RAPI_FINISHED_EVENT
                    try
                    {
                        if( outputAudio!=null )
                        {
                            if( DEBUG_FILE_POINTER ) System.out.println( "DEBUG: waveSize="+waveSize );
                            final int formSize = waveSize+46; // FORM size = AIFF + COMM chunk + SSND header + block size/offset
                            if( DEBUG_FILE_POINTER ) System.out.println( "DEBUG: Writing FORM Size "+formSize+" @ "+offsetFormatSize );
                            outputAudio.seek( offsetFormatSize );
                            outputAudio.writeInt( formSize );
                            final int numFrames = waveSize/2; // number of frames = 2 bytes per frame
                            if( DEBUG_FILE_POINTER ) System.out.println( "DEBUG: Writing Number of Frames "+numFrames+" @ "+offsetNumberFrames );
                            outputAudio.seek( offsetNumberFrames );
                            outputAudio.writeInt( numFrames );
                            final int ssndSize = waveSize+8; // SSND size = sound data plus 2 4-byte ints (block offset & block size)
                            if( DEBUG_FILE_POINTER ) System.out.println( "DEBUG: Writing SSND Size "+ssndSize+" @ "+offsetSsndSize );
                            outputAudio.seek( offsetSsndSize );
                            outputAudio.writeInt( ssndSize );
                            outputAudio.close();
                        }
                    }
                    catch( IOException error )
                    {
                        StringWriter sw=new StringWriter();
                        PrintWriter pw=new PrintWriter( sw );
                        error.printStackTrace( pw );
                        pw.flush();
                        err( "Failed to close audio file \""+filename+"\"\n"+
                            sw.toString() );
                    }
                    break;
            } //  end switch
        }
        catch( Exception error )
        {
            error.printStackTrace();
        }
    }
#endif



    }
}
