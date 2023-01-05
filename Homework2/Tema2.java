import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.nio.file.*;
import java.io.File;
import java.io.FileWriter;

public class Tema2 {
    
    public static void main(String[] args) throws IOException {
        if (args.length < 2) {
            System.out.println("Not enough parameters");
        }

        /* Extract input data */
        String orders = args[0] + "/orders.txt";
        String orderProducts = args[0] + "/order_products.txt";
        
        Integer P = Integer.valueOf(args[1]);
        File ordersFile = new File(orders);

        /* Create the output files*/
        FileWriter writerOrders = new FileWriter("orders_out.txt");
        FileWriter writerProducts = new FileWriter("order_products_out.txt");
        

        /* Find the size of the file in Bytes and divide it to P threads */
        Double ordersBytes =  new File(orders).length() * 1.0;
        Double chunk = Math.ceil(ordersBytes / P.doubleValue());
        Integer chunkBytes = chunk.intValue();

        /* Initialize the thread pool with P workers */
        ExecutorService pool = Executors.newFixedThreadPool(P);

        /* Each thread will read a chunk of bytes from the file */
        Integer startIndex = 0;
        Integer endIndex = ordersBytes.intValue() - 1;
        ArrayList<ArrayList<Integer>> bytesIndexes = new ArrayList<ArrayList<Integer>>();
        for (int i = 0; i < P; i++) {
            ArrayList<Integer> indexes = new ArrayList<Integer>();

            /* The start byte where thread i will start reading */
            indexes.add(startIndex);
            startIndex += chunkBytes.intValue() - 1;

            if (startIndex >= endIndex) {
                startIndex = endIndex;
            }
            /* The end byte where thread i will stop reading */
            indexes.add(startIndex);
            startIndex += 1;

            bytesIndexes.add(indexes);
        }

        /* Start reading in parralell orders files with P threads */
        Thread[] threads = new Thread[P];
        for (int i = 0; i < P; i++) {
        
            Integer start = bytesIndexes.get(i).get(0);
            Integer end = bytesIndexes.get(i).get(1);
            threads[i] = new MyThread(start, end, i, orders, pool, writerOrders, 
                                        writerProducts, orderProducts);
            threads[i].start();
        }

        /* Wait for all threads to finish */
        for (int i = 0; i < P; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        /* Close the thread pool */
        pool.shutdownNow();

        /* Close writers */
        writerOrders.close();
        writerProducts.close();
    }
}
