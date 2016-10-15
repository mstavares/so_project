CFLAGS = -Wall
LDFLAGS = -lpthread -luuid -lrt

orders: orders.c transaction.c
	gcc orders.c transaction.c -o orders  $(LDFLAGS)

clean:
	rm orders

