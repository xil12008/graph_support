package synergy.query;

import java.io.IOException;

// Websocket
import javax.websocket.OnClose;
import javax.websocket.OnMessage;
import javax.websocket.OnOpen;
import javax.websocket.OnError;
import javax.websocket.Session;
import javax.websocket.server.ServerEndpoint;

// JSON Parser
import org.json.*;

// Neo4j communication - libraries need to start a http request
import java.net.URL;
import java.net.HttpURLConnection;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.InputStreamReader;


@ServerEndpoint("/query")
public class WebsocketQuery {

	@OnOpen
	public void wsOpen() {
		System.out.println("New Incoming Connection.");
	}
	
	@OnClose
	public void wsClose(){
		System.out.println("Connection Closed.");
	}
	
    @OnMessage
    public void wsMessage(Session session, String msg, boolean last) {
    	
        try {
        	
            if (session.isOpen()) {
            	
            	String result = new String();
            	System.out.println(msg);
            	
            	// parse the incoming string as JSON Object
            	JSONObject jobj = new JSONObject(msg);
            	
            	// verify its format then get keyword for query
            	if ( jobj.has("type") ) {
            		
            		String t = jobj.get("type").toString();
            		
            		if( t.equals("query") && jobj.has("molecule") ) {
            			
            			String mol = jobj.get("molecule").toString();
            			
            			try {
            			  result = searchByMolecule(mol);
            			  
            			  // TODO: handle this exception - mainly due to http connection
            			} catch (Exception ioeee) {
            				ioeee.printStackTrace();
            			}
            		}
            	}
            	
            	// sending the result back
                session.getBasicRemote().sendText(result, last);
            }
            
        } catch (IOException e) {

        	e.printStackTrace();
        	
        	try {
                session.close();
            } catch (IOException e1) {
                e1.printStackTrace();
            }
        }
    }

    // when encounter websocket error - close the connection
    // have no business with Throwable yet
    @OnError
    public void wsError(Session session, Throwable exception) {
    	
    	System.err.println("Websocket Error.");
    	exception.printStackTrace();
    	
    	try {
    		session.close();
    	} catch (IOException e) {
    		e.printStackTrace();
    	}
    }
    
    // communicate with Neo4j Database
    // right now the input string should be a CYPHER QUERY instead of a molecule name
    private String searchByMolecule(String mol) throws Exception {
    	
    	// RESTful API
    	System.out.println(mol);
    	
    	// warp the Cypher Query as json object
    	String compose_request = "{\"statements\" : [ {\"statement\" : \"";
    	compose_request += mol;
    	compose_request += "\"} ]}";
    	
    	System.out.println(compose_request);
    	
    	// Construct a HTTP request
    	// TODO: stop hard coding the server address
    	String url = "http://ganxis.nest.rpi.edu:7474/db/data/transaction/commit";
    	URL obj = new URL(url);
    	HttpURLConnection con = (HttpURLConnection) obj.openConnection();
    	
    	/* As required by RESTful API from Neo4j
    	 * specify request method as POST
    	 *   data format(both in and out) is JSON
    	 *   providing authentication information as part of the header
    	 */
    	
    	// construct its header
    	con.setRequestMethod("POST");
    	con.setRequestProperty("Accept", "application/json; charset=UTF-8");
    	con.setRequestProperty("Content-Type", "application/json");
    	
    	// TODO: Stop hard coding the user name and password
    	// hard coded the user name and password(after encoded by base 64)
    	con.setRequestProperty("Authorization", "Basic bmVvNGo6dTJDcHlrZms=");
    	
    	// send the request
    	con.setDoOutput(true);
    	DataOutputStream wr = new DataOutputStream(con.getOutputStream());
    	wr.writeBytes(compose_request);
    	wr.flush();
    	wr.close();
    	
    	// retrieve result
    	BufferedReader in = new BufferedReader(
    			new InputStreamReader(con.getInputStream()));
    	
    	StringBuffer response = new StringBuffer();
    	String oneLine;
    	
    	while( (oneLine = in.readLine())!= null ) {
    		response.append(oneLine);
    	}
    	
    	in.close();
    	
    	return response.toString();
    }




}
