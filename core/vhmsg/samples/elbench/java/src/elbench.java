
import java.io.*;

import edu.usc.ict.vhmsg.*;


public class elbench implements MessageListener
{
   public static VHMsg vhmsg;

   public int numMessagesReceived = 0;
   public int m_testSpecialCases = 0;

   private boolean kbhit() 
   {
      try 
      {
         return ( System.in.available() != 0 );
      } 
      catch (IOException ioe) 
      {
      }
      return false;
   }

   public elbench( int receiveMode, int testSpecialCases )
   {
      System.out.println( "VHMSG_SERVER: " + System.getenv( "VHMSG_SERVER" ) );
      System.out.println( "VHMSG_SCOPE: " + System.getenv( "VHMSG_SCOPE" ) );


      vhmsg = new VHMsg();

      boolean ret = vhmsg.openConnection();
      if ( !ret )
      {
         System.out.println( "Connection error!" ); 
         return;
      }

      int NUM_MESSAGES = 20000;

      m_testSpecialCases = testSpecialCases;

      if ( receiveMode == 1 )
      {
         vhmsg.enableImmediateMethod();
         vhmsg.addMessageListener( this );
         vhmsg.subscribeMessage( "elbench" );

         System.out.println( "Receive Mode" );

         if ( testSpecialCases == 1 )
         {
            System.out.println( "Testing special case messages" );

            while ( !kbhit() )
            {
            }
         }
         else
         {
            long timeBefore = 0;
            long timeAfter;

            while ( !kbhit() )
            {
               // we've received our first message
               if ( numMessagesReceived > 0 && timeBefore == 0 )
               {
                  timeBefore = System.currentTimeMillis();
               }

               if ( numMessagesReceived >= NUM_MESSAGES )
               {
                  timeAfter = System.currentTimeMillis();

                  System.out.println( "Time to receive " + NUM_MESSAGES + " messages: " + ( timeAfter - timeBefore ) );

                  numMessagesReceived = 0;
                  timeBefore = 0;
               }
            }
         }
      }
      else
      {
         System.out.println( "Send Mode" );

         if ( testSpecialCases == 1 )
         {
            System.out.println( "Testing special case messages" );

            String s;

            s = "Test Message: 'Single-Quote'";
            System.out.println( "Sending " + s );
            vhmsg.sendMessage( "elbench " + s );

            s = "Test Message: \"Double-Quote\"";
            System.out.println( "Sending " + s );
            vhmsg.sendMessage( "elbench " + s );

            s = "Test Message: [Bracket]";
            System.out.println( "Sending " + s );
            vhmsg.sendMessage( "elbench " + s );

            s = "Test Message: {Brace}";
            System.out.println( "Sending " + s );
            vhmsg.sendMessage( "elbench " + s );

            s = "Test Message: \\Backslash\\";
            System.out.println( "Sending " + s );
            vhmsg.sendMessage( "elbench " + s );

            s = "Test Message: /Slash/";
            System.out.println( "Sending " + s );
            vhmsg.sendMessage( "elbench " + s );

            s = "Test Message: (Parenthesis)";
            System.out.println( "Sending " + s );
            vhmsg.sendMessage( "elbench " + s );

            s = "Test Message: <Angled Parens?>";
            System.out.println( "Sending " + s );
            vhmsg.sendMessage( "elbench " + s );

            s = "Test Message: `Dunno what these are called`";
            System.out.println( "Sending " + s );
            vhmsg.sendMessage( "elbench " + s );

            s = "Test Message: \"~!@#$%^&*()_+\" - top row special characters";
            System.out.println( "Sending " + s );
            vhmsg.sendMessage( "elbench " + s );
         }
         else
         {
            long timeBefore = System.currentTimeMillis();

            for ( int i = 0; i < NUM_MESSAGES; i++ )
            {
               vhmsg.sendMessage( "elbench " + i + " Test Message" );

               if ( i % 2000 == 0 )
               {
                  System.out.println( i + " messages sent" );
               }
            }

            long timeAfter = System.currentTimeMillis();

            System.out.println( "Time to send " + NUM_MESSAGES + " messages: " + ( timeAfter - timeBefore ) );
         }
      }
   }


   public void messageAction( MessageEvent e )
   {
      //System.out.println( "Received Message '" + e.toString() + "'" );

      if ( m_testSpecialCases == 1 )
      {
         System.out.println( "received - '" + e.toString() + "'" );
      }
      else
      {
         numMessagesReceived++;

         if ( numMessagesReceived % 2000 == 0 )
         {
            System.out.println( numMessagesReceived + " messages received - '" + e.toString() + "'" );
         }
      }
   }


   public static void main( String[] args )
   {
      int receiveMode = 0;
      int testSpecialCases = 0;

      if ( args.length > 0 )
      {
         receiveMode = Integer.valueOf( args[ 0 ] ).intValue();
      }
      if ( args.length > 1 )
      {
         testSpecialCases = Integer.valueOf( args[ 1 ] ).intValue();
      }

      elbench elbenchObj = new elbench( receiveMode, testSpecialCases );

      System.exit( 0 );
   }
}
