package controls;
import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.application.Application;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.input.MouseButton;
import javafx.scene.paint.*;
import javafx.geometry.Insets;
import javafx.geometry.Orientation;
import javafx.geometry.Pos;
import javafx.scene.control.*;
import javafx.scene.Scene; // the space where all widgets are
import javafx.scene.control.MenuBar;
import javafx.scene.layout.*;
import javafx.stage.Stage; // the actual window
import entities.*;
import javafx.util.Duration;
import java.io.File;
import java.util.AbstractMap;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;

/**
 * Main class of an application.
 * @author Michal Rein, Ondřej Motyčka
 */
public class Main extends Application {

    /** Main stage object. */
    Stage window;
    private static File fp = null;

    /**
     * Main.
     * @param args Arguments.
     */
    public static void main(String[] args)
    {
        launch(args);
    }

    /**
     * Controls procedure of creating application layout. All necessary objects and containers are created,
     * application loop and events are handled by JavaFX Timeline object.
     * @param primaryStage Main stage object.
     * @throws Exception Exception.
     */
    @Override
    public void start(Stage primaryStage) throws Exception {

        window = primaryStage;
        window.setTitle("IJA-Project");

        // when red cross is clicked on
        window.setOnCloseRequest(e -> {
            e.consume(); // takes responsibility for ending the whole program (closing window would otherwise close program every time no matter what)
            closeProgram();
        });

        // create layout
        BorderPane borderPane = new BorderPane(); // main layout
        MenuBar menuBar = new MenuBar(); // menu bar

        VBox statusBarWrap = new VBox();
            GridPane timeStatusBar = new GridPane();
                timeStatusBar.setHgap(65);
                timeStatusBar.setVgap(5);
                timeStatusBar.prefWidth(250);
                statusBarWrap.setMargin(timeStatusBar, new Insets(10, 0, 10, 0));
                statusBarWrap.getChildren().add(timeStatusBar);
            GridPane lineStatusBar = new GridPane(); // left status bar (display of statistics, line info, position...)
                lineStatusBar.setHgap(10); // set horizontal gap scale
                lineStatusBar.setVgap(5); // set vertical gap scale
                lineStatusBar.prefWidth(250);
                statusBarWrap.setMargin(lineStatusBar, new Insets(10, 0, 10, 0));
                statusBarWrap.getChildren().add(lineStatusBar);
            GridPane streetStatusBar = new GridPane();
                streetStatusBar.setHgap(35);
                streetStatusBar.setVgap(5);
                streetStatusBar.prefWidth(250);
                statusBarWrap.setMargin(streetStatusBar, new Insets(10, 0, 10, 0));
                statusBarWrap.getChildren().add(streetStatusBar);
            GridPane stopStatusBar = new GridPane();
                stopStatusBar.setHgap(55);
                stopStatusBar.setVgap(5);
                stopStatusBar.prefWidth(250);
                statusBarWrap.setMargin(stopStatusBar, new Insets(10, 0, 0, 0));
                statusBarWrap.getChildren().add(stopStatusBar);
            VBox statusBarFeatures = new VBox();
                statusBarFeatures.setAlignment(Pos.CENTER_LEFT);
                statusBarFeatures.setSpacing(10);
                statusBarWrap.setMargin(statusBarFeatures, new Insets(0, 0, 10, 0));
                statusBarWrap.getChildren().add(statusBarFeatures);
            statusBarWrap.setStyle("-fx-background-color: white;");
            statusBarWrap.setStyle("-fx-width: 200px;");

            // create ScrollPane for status bar
            ScrollPane statusScrollPane = new ScrollPane();
            statusScrollPane.setContent(statusBarWrap);
            statusScrollPane.setStyle("-fx-background-color:WHITE");


        HBox mapCanvasWrap = new HBox();
            // left separator
            Separator leftCenterSep = new Separator(Orientation.VERTICAL);
                leftCenterSep.setPadding(new Insets(0, 0, 0, 0));
                mapCanvasWrap.getChildren().add(leftCenterSep);
            // map canvas
            MapPane mapCanvas = new MapPane();
                //BorderPane.setMargin(mapCanvas, insets);
                mapCanvasWrap.setPrefWidth(800);
                mapCanvasWrap.getChildren().add(mapCanvas);

        // slider on the right side
        VBox sliderBox = new VBox();
            sliderBox.setAlignment(Pos.CENTER);
            sliderBox.prefWidth(10);

        borderPane.setCenter(mapCanvasWrap);
        borderPane.setRight(sliderBox);
        //borderPane.setLeft(statusBarWrap);
        borderPane.setLeft(statusScrollPane);
        borderPane.setTop(menuBar);

        setUserAgentStylesheet(STYLESHEET_CASPIAN);

        /*PROGRAM MENU, JSON LOAD*/
        if (fp == null){
            fp = ShowBox.displayProgramMenu();
            if (fp == null){
                window.close();
                System.exit(0);
            }
        }


        /*MENU*/
        CustomMenu menu = new CustomMenu(menuBar);

        /*MAP*/
        StreetMap streetMap = new StreetMap(mapCanvas, fp);

        /*ZOOM*/
        ZoomSlider zoomSlider = new ZoomSlider(sliderBox, mapCanvas);

        // create array for storing vehicles
        List<Vehicle> vehicleArray = new ArrayList<>();
        List<TransportLine> transportLines = streetMap.getTransportLineList();
        for (TransportLine transportLine : transportLines){

            Random random = new Random();
            float r = random.nextFloat();
            float g = random.nextFloat();
            float b = random.nextFloat();

            for (AbstractMap.SimpleImmutableEntry<String, String> vehicle : transportLine.getVehicleList()){
                vehicleArray.add(new Vehicle(transportLine, new Color(r, g, b, 1), vehicle.getKey(), Time.convertStringToTime(vehicle.getValue())));
            }

        }

        // add all vehicles to the map canvas
        vehicleArray.forEach((vehicle) -> mapCanvas.getChildren().addAll(vehicle, vehicle.getLineTextObj()));

        /*STATUS BAR*/
        StatusBarLine statusLine = new StatusBarLine(lineStatusBar, vehicleArray);
        StatusBarStreet statusStreet = new StatusBarStreet(streetStatusBar, streetMap);
        StatusBarStop statusStop = new StatusBarStop(stopStatusBar, streetMap);
        StatusBarFeatures statusButtons = new StatusBarFeatures(statusBarFeatures, streetMap);
        StatusBarTime statusTime = new StatusBarTime(timeStatusBar);

        /*SIMULATED TIME*/
        Time appTime = new Time(0, statusTime);
        statusTime.setTimeHandle(appTime);
        statusLine.setTimeHandle(appTime);

        // For each vehicle, provide status bar pointer, create time table and set events
        vehicleArray.forEach((vehicle -> {
            vehicle.setStatusBarLine(statusLine);
            vehicle.createTimetable();
            vehicle.setEvents();
        }));

        // For each street (it's drawn line), set on click event
        streetMap.getStreetList().forEach((street) -> street.getStrLine().setOnMouseClicked(e -> statusStreet.switchStreet(street)));

        // for each stop (cycling through every street), set on click event
        streetMap.getStreetList().forEach((street -> street.getStops().forEach(stop -> stop.getStopMark().setOnMouseClicked(e -> statusStop.switchStop(stop)))));

        // set right mouse click on entire map canvas for removing selection
        mapCanvas.setOnMouseClicked(e -> {
            if (e.getButton() == MouseButton.SECONDARY){
                statusLine.resetStatus();
                statusStreet.resetStatus();
                statusStop.resetStatus();
            }
        });

        // set 1050x600 scene and show
        Scene scene = new Scene(borderPane, 1050, 600);
        window.setScene(scene);
        window.show();

        // timeline is responsible for application loop that handles events
        // new event is evoked every 10 milliseconds
        Timeline timeline = new Timeline(new KeyFrame(Duration.millis(10),
                new EventHandler<ActionEvent>() {

                    // position recalculation for all vehicles, update of simulated time
                    @Override
                    public void handle(ActionEvent event) {

                        // calculation of vehicle trajectory should be made every second (10ms * 100 = 1 second)
                        if (appTime.getDelayCounter() >= 100/appTime.getSpeedRatio()){

                            // draw vehicle
                            vehicleArray.forEach(vehicle -> {

                                // check if vehicle has been deployed
                                if (vehicle.isDeployed()){
                                    vehicle.calculateTrajectory();
                                // stay dormant and compare deployment time with an app time
                                } else {
                                    vehicle.checkDeployTime();
                                }
                            });
                            appTime.updateTime();
                            appTime.resetDelayCounter();
                        }
                        // increment delay counter
                        appTime.incrementDelayCounter();
                    }
                }));

        // set cycle to indefinite time period
        timeline.setCycleCount(Timeline.INDEFINITE);
        timeline.play(); // play :)
    }

    /**
     * Close program, ask user if he really wants to exit.
     */
    private void closeProgram(){
        boolean answer = ConfirmBox.display("Exit?", "Sure you want to exit?");
        if (answer){
            window.close();
            System.exit(0);
        }
    }

}
