package tw.idv.jenhu.demo;

import com.fasterxml.jackson.databind.ObjectMapper;
import javax.ws.rs.ApplicationPath;
import org.glassfish.jersey.server.ResourceConfig;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

/**
 * Jersey設定類別.
 */
@Component
@ApplicationPath("/")
public class JerseyConfig extends ResourceConfig {
  /**
   * Default constructor.
   *
   * @param objectMapper ObjectMapper instance
   */
  @Autowired
  public JerseyConfig(ObjectMapper objectMapper) {

    // register endpoints
    // use packages scan in Spring Boot Standalone JAR will fails
    // packages("com.hwacom.innodiv.aiot.user.controller");
    register(MainController.class);

    // register jackson for json
    // register(new ObjectMapperContextResolver(objectMapper));


  }
}
