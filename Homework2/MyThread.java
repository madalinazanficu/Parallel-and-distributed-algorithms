import java.nio.file.*;
import java.util.stream.*;
import java.io.IOException;

public class MyThread extends Thread {

    long start;
    long end;
    int id;

    public MyThread(long start, long end, int id) {
        this.start = start;
        this.end = end;
        this.id = id;
    }

    @Override
    public void run() {
        System.out.println("Thread " + id + " started");

        
        /*String line;
        try (Stream<String> lines = Files.lines(Main.ordersPath)) {
            line = lines.skip(start).findFirst().get();
            System.out.println(line);
        } catch(IOException e) {
            e.printStackTrace();
        }*/

        try (Stream<String> commandStream = Files.lines(Main.ordersPath)
                                                 .skip(start).limit(Main.count)) {

            System.out.println("Thread " + id + " started");
            for (String command : (Iterable<String>) commandStream::iterator) {
                System.out.println(command);
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    
}
