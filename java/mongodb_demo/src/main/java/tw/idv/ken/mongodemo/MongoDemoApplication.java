package tw.idv.ken.mongodemo;

import com.github.javafaker.service.FakeValuesService;
import com.github.javafaker.service.RandomService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.CommandLineRunner;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

import java.util.LinkedList;
import java.util.List;
import java.util.Locale;

@SpringBootApplication
public class MongoDemoApplication implements CommandLineRunner {
	@Autowired
	private CustomerRepository repository;

	public static void main(String[] args) {
		SpringApplication.run(MongoDemoApplication.class, args);
	}

	@Override
	public void run(String... args) throws Exception {

		repository.deleteAll();

		// save a couple of customers
		repository.save(new Customer("Alice", "Smith"));
		repository.save(new Customer("Bob", "Smith"));

		// fetch all customers
		System.out.println("Customers found with findAll():");
		System.out.println("-------------------------------");
		for (Customer customer : repository.findAll()) {
			System.out.println(customer);
		}
		System.out.println();

		// fetch an individual customer
		System.out.println("Customer found with findByFirstName('Alice'):");
		System.out.println("--------------------------------");
		System.out.println(repository.findByFirstName("Alice"));

		System.out.println("Customers found with findByLastName('Smith'):");
		System.out.println("--------------------------------");
		for (Customer customer : repository.findByLastName("Smith")) {
			System.out.println(customer);
		}

		FakeValuesService fakeValuesService = new FakeValuesService(
				new Locale("en-GB"), new RandomService());

		List<Customer> customerList = new LinkedList<>();
		for(int i=0; i<100000; i++) {
			//String email = fakeValuesService.bothify("????##@gmail.com");
			customerList.add(new Customer(fakeValuesService.letterify("????"),
					fakeValuesService.letterify("?????")));
		}

		System.out.println(System.currentTimeMillis());
		customerList.parallelStream().forEach(repository::save);
		System.out.println(System.currentTimeMillis());
		System.out.println("inserted customers: " + repository.findAll().size());
		repository.deleteAll();
	}

}
