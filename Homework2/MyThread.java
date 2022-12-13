import java.nio.file.*;
import java.util.ArrayList;
import java.util.concurrent.Semaphore;
import java.util.stream.*;
import java.io.IOException;

public class MyThread extends Thread {

    // The current thread is responsible for landing tasks for these commands
    ArrayList<String> orderdsIds = new ArrayList<>();
    ArrayList<Integer> quantities = new ArrayList<>();
    ArrayList<Semaphore> semaphores = new ArrayList<>();
    long start;
    long end;
    int id;

    public MyThread(long start, long end, int id) {
        this.start = start;
        this.end = end;
        this.id = id;
    }

    /* Extract the commands assigned to the current thread
        Read only the lines that are assigned to this thread
       Read from orderProducts file and add writing tasks to the thread pool
     */
    @Override
    public void run() {
        try (Stream<String> commandStream = Files.lines(Tema2.ordersPath)
                                                 .skip(start).limit(Tema2.count)) {

            for (String command : (Iterable<String>) commandStream::iterator) {
                String[] commadParts = command.split(",");

                String orderId = commadParts[0];
                Integer quantity = Integer.valueOf(commadParts[1]);
                Semaphore semaphore = new Semaphore(-(quantity - 1));

                orderdsIds.add(orderId);
                quantities.add(quantity);
                semaphores.add(semaphore);
            }

            // Land new tasks
            for (int i = 0; i < orderdsIds.size(); i++) {
                if (quantities.get(i) > 0) {
                
                    // Sumbit the first task for the current order
                    MyTask task = new MyTask(orderdsIds.get(i), semaphores.get(i), 0);
                    Tema2.pool.submit(task);

                    // Wait for the products to be delivered
                    semaphores.get(i).acquire();

                    // The products were delivered => the whole order is shipped
                    CommandCompleted(orderdsIds.get(i), quantities.get(i));
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public void CommandCompleted(String orderId, Integer quantity) {
        synchronized(Tema2.writerOrders) {
            try {
                Tema2.writerOrders.write(orderId + "," + quantity +  "," + "shipped" + "\n");
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
    
}
    

