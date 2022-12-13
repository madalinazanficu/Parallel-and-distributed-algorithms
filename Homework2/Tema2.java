import java.io.IOException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.nio.file.*;
import java.io.File;
import java.io.FileWriter;

public class Tema2 {
    static String orders;
    static String orderProducts;
    
    static Integer P;
    static Integer N = 0;
    static Integer count;
    static ExecutorService pool;
    
    static Path ordersPath;
    static long ordersSize;

    static File productsFile;
    static Path productsPath;
    static long productsSize;

    static String ordersOut;
    static String productsOut;
    static FileWriter writerOrders;
    static FileWriter writerProducts;

    static Integer ordersLock = 0;
    static Integer productsLock = 0;

    public static void main(String[] args) throws IOException {
        if (args.length < 2) {
            System.out.println("Not enough parameters");
        }

        // Extract input data
        orders = args[0] + "/orders.txt";
        orderProducts = args[0] + "/order_products.txt";
        P = Integer.valueOf(args[1]);

        // Create the output directory
        // String number = args[0].substring(args[0].length() - 1);
        ordersOut = "orders_out.txt";
        productsOut = "order_products_out.txt";
        writerOrders = new FileWriter(ordersOut);
        writerProducts = new FileWriter(productsOut);
        
        // Create the orderProducts file
        productsFile = new File(orders);
        
        // Count the number of commands from orders.txt
        ordersPath = Paths.get(orders);
        ordersSize = Files.lines(ordersPath).count();

        // Count the number of products from orderProducts.txt
        productsPath = Paths.get(orderProducts);
        productsSize = Files.lines(productsPath).count();
        
        // Distribute equally the number of commands to P theards
        Thread[] threads = new Thread[P];
        if (ordersSize > P) {
            Double division = Math.ceil(ordersSize / P.doubleValue());
            count  = division.intValue();
        } else {
            count = 1;
        }     

        // Initialize the thread pool with P workers
        pool = Executors.newFixedThreadPool(P);

        for (int i = 0; i < P; i++) {
        
            // Start the thread if there are commands to be executed
            if (i * count < ordersSize) {
                long start = i * count;
                long end = start + count - 1;

                if (end >= ordersSize) {
                    end = ordersSize - 1;
                }
                N++;
                threads[i] = new MyThread(start, end, i);
                threads[i].start();
            }
        }
        for (int i = 0; i < N; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        // Close the thread pool
        pool.shutdownNow();

        // Close the writer
        writerOrders.close();
        writerProducts.close();
    }
}
