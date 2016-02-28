package kevin.heypebble;

import android.app.SearchManager;
import android.content.Intent;
import android.os.AsyncTask;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.SearchView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.UUID;

import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.*;

public class MainActivity extends AppCompatActivity {

    private final static UUID PEBBLE_APP_UUID = UUID.fromString("f19ceb67-6d45-43a0-8e69-f56dd53c6197");
    private static final int KEY_DATA = 0;
    protected TextView selection;
    protected SearchView data;
    protected PebbleDictionary result;
    protected PebbleKit.PebbleDataReceiver receiver;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        selection = (TextView) findViewById(R.id.textView);
        data = (SearchView) findViewById(R.id.searchView);
        Button submit = (Button)this.findViewById(R.id.search);
        PebbleKit.startAppOnPebble(getApplicationContext(), PEBBLE_APP_UUID);
        result = new PebbleDictionary();
    }


    public void searchOnClick(View view) {
        String query = data.getQuery().toString();
        Reader reader = new Reader();
        reader.execute(query);
    }

    public class Reader extends AsyncTask<String, Void, String> {

        protected final String key = "AIzaSyD9ukYTAgormeoWvp_EikfPnpWW8DXTgxU";
        public String output;

        @Override
        protected String doInBackground(String... params) {
            boolean found = false;
            try{

                final URL url = new URL(
                        "https://www.googleapis.com/customsearch/v1?key="+key+"&cx=013036536707430787589:_pqjad5hr1a&q="+params[0]+"&alt=json");
                HttpURLConnection conn = (HttpURLConnection) url.openConnection();
                conn.setRequestMethod("GET");
                conn.setRequestProperty("Accept", "application/json");
                BufferedReader br = new BufferedReader(new InputStreamReader((conn.getInputStream())));
                while ((output = br.readLine()) != null) {
                    if(output.contains("\"displayLink\": \"en.wikipedia.org\"")){
                        found = true;
                    }
                    if(found){
                        String[] split = br.readLine().split("\"snippet\": \"");
                        output = split[1];;
                        result.addString(KEY_DATA, output);
                        PebbleKit.sendDataToPebble(getApplicationContext(), PEBBLE_APP_UUID, result);
                        found = false;
                    }
                }
                conn.disconnect();
                br.close();
            }catch(Exception e) {
                e.printStackTrace();
            }
            return null;
        }

        protected void onPostExecute(String s){
            super.onPostExecute(s);
        }
    }

}
