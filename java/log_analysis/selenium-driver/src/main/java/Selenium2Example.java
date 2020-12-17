import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import org.openqa.selenium.chrome.ChromeDriver;
import org.openqa.selenium.firefox.FirefoxDriver;
import org.openqa.selenium.remote.DesiredCapabilities;

import java.time.LocalDateTime;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Collectors;
import java.util.concurrent.TimeUnit;

public class Selenium2Example {
    /**
     * Constant to avoid long url.
     */
    public static final int URL_MAX_LANGTH = 120;
    /**
     * Selenium WebDriver instance.
     */
    private WebDriver driver;
    /**
     * Base url of the web site we are interested.
     */
    private String websiteBaseUrl = "";
    /**
     * Use this token as a must-have string to prevent too many external links.
     */
    private String urlToken = "";
    /**
     * Visit limit, -1 means no limit.
     */
    private int visits = -1;
    /**
     * Time interval in milliseconds between each visit, default to 3000.
     */
    private int interval = 3000;

    public Selenium2Example(WebDriver driver, String urlToken) {
        this.driver = driver;
        this.urlToken = urlToken;
    }

    public static void main(String[] args) throws InterruptedException {
        //specify chromedriver location
        //System.setProperty("webdriver.chrome.driver", "/Users//huzhongweidev-libaries/chromedriver");
        //WebDriver driver = new ChromeDriver();

        //firefox
        System.setProperty("webdriver.gecko.driver", "/Users/huzhongwei/dev-libaries/geckodriver");
        DesiredCapabilities capabilities = DesiredCapabilities.firefox();
        capabilities.setCapability("marionette", true);
        WebDriver driver = new FirefoxDriver(capabilities);

        driver.manage().deleteAllCookies();
        driver.manage().timeouts().implicitlyWait(2000, TimeUnit.MILLISECONDS);
        driver.manage().timeouts().pageLoadTimeout(40, TimeUnit.SECONDS);
        driver.manage().timeouts().setScriptTimeout(60, TimeUnit.SECONDS);

        LocalDateTime beginTime = LocalDateTime.now();

        // specify the url by command-line argument
        driver.get(args[0]);

        //create instance
        Selenium2Example instance = new Selenium2Example(driver, args[1]);

        if (args.length >= 3) {
            instance.setVisits(Integer.parseInt(args[2]));
        }

        if (args.length == 4) {
            instance.setInterval(Integer.parseInt(args[3]));
        }

        // start recursive linkText
        instance.visitAllAnchorTags();

        //Close the browser
        driver.quit();

        ZoneId zoneId = ZoneId.of("Asia/Taipei");
        LocalDateTime endTime = LocalDateTime.now();

        System.out.println(String.format("%s From %s(%s) To %s(%s)", args[0],
                beginTime.format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss")),
                beginTime.atZone(zoneId).toEpochSecond(),
                endTime.format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss")),
                endTime.atZone(zoneId).toEpochSecond()
        ));
    }

    /**
     * Visit all anchor tags existed in homepage.
     */
    private void visitAllAnchorTags() {
        String[] links = null;
        int linksCount = 0;

        List<WebElement> linksize = driver.findElements(By.tagName("a"))
                .stream().filter(l -> l.getAttribute("href") != null &&
                        l.getAttribute("href").length() < URL_MAX_LANGTH &&
                        l.getAttribute("href").startsWith("http") &&
                        l.getAttribute("href").contains(this.urlToken)
                ).collect(Collectors.toList());
        linksCount = computeVisitLimit(linksize.size());

        System.out.println("Total no of links Available: " + linksCount);
        links = new String[linksCount];

        for (int i = 0; i < linksCount; i++) {
            links[i] = linksize.get(i).getAttribute("href").replaceAll("#$", "").replaceAll("/$", "");
        }

        Arrays.stream(links).distinct().forEach(link -> {
            //if (link.startsWith(websiteBaseUrl) && !link.contains("#")) {
            if (link.contains(this.urlToken) && !link.contains("#")) {
                driver.navigate().to(link);
                try {
                    Thread.sleep(this.interval);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    private int computeVisitLimit(int linksCount) {
        return (this.visits > -1 && this.visits < linksCount) ? this.visits : linksCount;
    }

    public void setVisits(int visits) {
        this.visits = visits;
    }

    public void setInterval(int interval) {
        this.interval = interval;
    }
}

