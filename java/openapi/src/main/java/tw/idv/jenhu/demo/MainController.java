package tw.idv.jenhu.demo;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.ws.rs.Consumes;
import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;
import java.util.Arrays;
import java.util.List;


/**
 * Company相關的rest controller.
 */
@Component
@Path("/")
public class MainController {

  /**
   * 建立租用公司並取得系統管理員的使用者資料.
   *
   * @param customer 租用客戶資料
   * @return 系統管理員的使用者資料
   */
  @GET
  @Path("/users")
  @Produces(MediaType.APPLICATION_JSON)
  public Response getAllUsers() {
      User ken = new User();
      ken.setName("Ken Hu");
      ken.setAge(51);
      User stefanie = new User();
      stefanie.setName("Stefanie Yen");
      stefanie.setAge(51);
      List<User> users = Arrays.asList(ken, stefanie);

      return Response.ok().entity(users).build();
  }
}
