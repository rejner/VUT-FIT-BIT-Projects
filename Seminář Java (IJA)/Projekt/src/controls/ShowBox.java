package controls;

import entities.Stop;
import entities.Street;
import entities.Vehicle;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.geometry.Orientation;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.ScrollPane;
import javafx.scene.control.Separator;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.VBox;
import javafx.scene.text.TextAlignment;
import javafx.stage.FileChooser;
import javafx.stage.Modality;
import javafx.stage.Stage;
import java.io.File;
import java.util.AbstractMap;

/**
 * Class ShowBox implements some pop-up windows, which are called by user.
 * @author Michal Rein, Ondřej Motyčka
 */

public class ShowBox {

    static File file = null;

    /**
     * Displays timetable of given vehicle (Called by event in Show route button).
     * @param line Number of displayed line.
     * @param vehicle Reference on specific vehicle (class) which timetable should be displayed.
     */
    public static void displayRoute(String line, Vehicle vehicle){

        // new stage
        Stage window = new Stage();

        // route is displayed as grid layout
        GridPane grid = new GridPane();
        grid.setPadding(new Insets(10, 10, 10, 10)); // inset = object for padding
        grid.setVgap(8); // vertical gap of cells
        grid.setHgap(10); // horizontal gap of cells
        window.initModality(Modality.APPLICATION_MODAL); // block input event with other windows until this one is closed
        window.setTitle("Line " + line);
        window.setMinWidth(250);
        grid.setAlignment(Pos.CENTER);

        int rowIndex = 0; // index for indexing rows of grid
        int elementIndex = 0; // index for getting timetable values

        // if vehicle is moving in reversal way, get index of last element in timetable
        if (vehicle.getReverse()){
            elementIndex = vehicle.getTimetable().size() - 1;
        }

        // iterate through vehicle's route, associate timetable values with stops, skip any auxiliary stop
        for (AbstractMap.SimpleImmutableEntry<Street, Stop> stop : vehicle.getRoute()){

           if (stop.getValue().getId().equals("auxStop")) continue; // we can skip auxiliary stops

            Label labelIndex = new Label();
            labelIndex.setText(elementIndex + ": ");
            GridPane.setConstraints(labelIndex, 0, rowIndex);

            Label streetName = new Label();
            streetName.setText("Street: " + stop.getKey().getId());
            GridPane.setConstraints(streetName, 1, rowIndex++);

            Label stopName = new Label();
            stopName.setText("Stop: " + stop.getValue().getId());
            GridPane.setConstraints(stopName, 1, rowIndex++);

            Label estimatedArrivalTime = new Label();
            estimatedArrivalTime.setText("Estimated arrival: " + vehicle.getTimetable().get(elementIndex).getKey());
            GridPane.setConstraints(estimatedArrivalTime, 1, rowIndex++);

            Separator separator = new Separator(Orientation.HORIZONTAL);
            GridPane.setConstraints(separator, 0, rowIndex++);
            GridPane.setColumnSpan(separator, 3);

            grid.getChildren().addAll(labelIndex, streetName, stopName, estimatedArrivalTime, separator);

            // increment or decrement element index, it depends on whether it's moving reversal or not
            if (vehicle.getReverse()){
                elementIndex--;
            } else {
                elementIndex++;
            }

        }

        Scene scene = new Scene(grid);
        window.setScene(scene);
        window.showAndWait(); // requires to be closed before returning to caller
    }

    /**
     * Opening menu which pop-ups at the beginning of program execution. User has options to choose which JSON map reference
     * should be used to render map, or use sample default JSON.
     * @return File in JSON format containing information about map layout and transport lines.
     */
    public static File displayProgramMenu(){

        Stage window = new Stage();
        window.setTitle("IJA-Project Menu");
        window.setWidth(300);
        window.setHeight(300);

        VBox container = new VBox();
        container.setAlignment(Pos.CENTER);
        Insets defInsets = new Insets(5, 10, 5, 10);

        Label welcomeMsg = new Label("Welcome to our App!\nChoose one of following options: ");
            welcomeMsg.setTextAlignment(TextAlignment.CENTER);
            welcomeMsg.setStyle("-fx-padding: 10px; -fx-border-insets: 5px; -fx-background-insets: 5px;");
        Button loadRefFileBtn = new Button("Load reference");
            loadRefFileBtn.setPadding(defInsets);
            loadRefFileBtn.setMaxSize(100, 50);
            loadRefFileBtn.setStyle("-fx-padding: 10px; -fx-border-insets: 5px; -fx-background-insets: 5px;");
            loadRefFileBtn.setOnAction(e -> {
                file = new File("data/examples/example.json");
                window.close();
            });
        Button loadFileBtn = new Button("Load file");
            loadFileBtn.setPadding(defInsets);
            loadFileBtn.setMaxSize(100, 50);
            loadFileBtn.setStyle("-fx-padding: 10px; -fx-border-insets: 5px; -fx-background-insets: 5px;");
            loadFileBtn.setOnAction(e -> {
                // get JSON map representation from user
                final FileChooser fileChooser = new FileChooser();
                File fp = fileChooser.showOpenDialog(window);
                if (fp == null){
                    file = null;
                } else {
                    file = fp;
                    window.close();
                }

            });
        Button closeBtn = new Button("Close");
            closeBtn.setPadding(defInsets);
            closeBtn.setMaxSize(100, 50);
            closeBtn.setStyle("-fx-padding: 10px; -fx-border-insets: 5px; -fx-background-insets: 5px;");
            closeBtn.setOnAction(new EventHandler<ActionEvent>() {
                @Override
                public void handle(ActionEvent event) {
                    boolean answer = ConfirmBox.display("Exit?", "Sure you want to exit?");
                    if (answer){
                        window.close();
                    }
                }
            });
        Button helpBtn = new Button("Help");
            helpBtn.setPadding(defInsets);
            helpBtn.setMaxSize(100, 50);
            helpBtn.setStyle("-fx-padding: 10px; -fx-border-insets: 5px; -fx-background-insets: 5px;");
            helpBtn.setOnAction(new EventHandler<ActionEvent>() {
                @Override
                public void handle(ActionEvent event) {
                    ShowBox.displayHelp();
                }
            });

        container.getChildren().addAll(welcomeMsg, loadRefFileBtn, loadFileBtn, helpBtn, closeBtn);

        Scene scene = new Scene(container);
        window.setScene(scene);
        window.showAndWait();

        return file;
    }

    public static void displayHelp(){

        // new stage
        Stage window = new Stage();
        window.setWidth(700);
        window.setHeight(600);
        window.setTitle("Help");

        Label msg = new Label("Welcome to our app!\n" +
                                    "Here you will find all the information needed for operating this application!\n\n" +
                                    "----------------\n" +
                                    " CONTROLS\n" +
                                    "----------------\n" +
                                    " - Entire map can be moved by left clicking on the map's surface and dragging it into desired direction.\n" +
                                    " - All entities (streets/vehicles/stops) are clickable objects. Clicking on these objects will select them and display\n" +
                                    "   information about them on the status bar, which is located in the left part of the app.\n" +
                                    " - Selection of entities can be undone by right click on the map.\n" +
                                    " - Map can also be zoomed by moving the slide bar on the right.\n\n" +
                                    "------------------\n" +
                                    "  STATUS BAR\n" +
                                    "------------------\n" +
                                    " - Here, you can find all information that you need.\n" +
                                    " - It splits into 5 sections:\n" +
                                    "       Time:\n" +
                                    "           - Time section hold information about application simulated time.\n" +
                                    "           - You can speed up pace of time by selecting one of preset modifiers from the choice box.\n" +
                                    "       Vehicle:\n" +
                                    "           - Here you will see info about a selected vehicle.\n" +
                                    "           - Vehicle can be selected either by clicking it directly on the map or from choice box.\n" +
                                    "           - If you want to view vehicle's time table, just click on 'Show route' button!\n" +
                                    "       Street:\n" +
                                    "           - Displays information about selected street.\n" +
                                    "       Stop:\n" +
                                    "           - Displays information about selected stop.\n" +
                                    "       Features:\n" +
                                    "           - Contains check boxes for turning on/off some special features.\n\n\n" +
                                    "---------------------------------------\n" +
                                    "  MAP REPRESENTATION FORMAT\n" +
                                    "---------------------------------------\n" +
                                    " - To show a custom map and lines, map representation must be created using JSON format including certain objects.\n" +
                                    " - Every map representation should be structured like this sample:\n\n" +
                                    "{\n" +
                                    "    \"streets\": [\n" +
                                    "        {\n" +
                                    "            \"streetName\": \"Example street\",\n" +
                                    "            \"begin\": [ 100, 100 ],\n" +
                                    "            \"end\": [ 500, 100 ],\n" +
                                    "            \"congestion\": 2,\n" +
                                    "            \"stops\": [\n" +
                                    "                {\n" +
                                    "                    \"stopName\": \"Example stop 1\",\n" +
                                    "                    \"coordinates\": [ 200, 100 ]\n" +
                                    "                },\n" +
                                    "                {\n" +
                                    "                    \"stopName\": \"Example stop 2\",\n" +
                                    "                    \"coordinates\": [ 400, 100 ]\n" +
                                    "                }\n" +
                                    "            ]\n" +
                                    "        }\n" +
                                    "     ],\n" +
                                    "    \"lines\": [\n" +
                                    "        {\n" +
                                    "            \"lineNumber\": \"42\",\n" +
                                    "            \"route\": [\n" +
                                    "                {\n" +
                                    "                    \"streetName\": \"Example street\",\n" +
                                    "                    \"stops\": [ \"Example stop 1\", \"Example stop 2\" ]\n" +
                                    "                }\n" +
                                    "            ],\n" +
                                    "            \"vehicles\": [\n" +
                                    "                {\n" +
                                    "                    \"id\": 101,\n" +
                                    "                    \"deploy\": \"00:00:10\"\n" +
                                    "                },\n" +
                                    "                {\n" +
                                    "                    \"id\": 102,\n" +
                                    "                    \"deploy\": \"00:00:30\"\n" +
                                    "                }\n" +
                                    "            ]\n" +
                                    "         }\n" +
                                    "      ]\n" +
                                    "}\n\n" +
                                    " - This example will create a street with the beginning coordinates at [100, 100] and end at [500, 100], which\n" +
                                    "   has level of congestion set to 2 (vehicles will move 4x slower) and contains 2 stops (Example stop 1 and Example stop 2).\n" +
                                    " - Also, this representation contains another objects with information about lines. Each defined line has it's line number,\n" +
                                    "   route, which is described only by street and stop names (no coordinates needed, since all stops were already created in\n" +
                                    "   streets object) and array of vehicles, where each vehicle has it's own identifier and time of deployment, that\n" +
                                    "   has to be given in HH:MM:SS time format.\n" +
                                    "\n\n Authors: Michal Rein and Ondřej Motyčka\n");

        GridPane.setConstraints(msg, 0 ,0);
        GridPane grid = new GridPane();
        grid.setPadding(new Insets(20));
        grid.getChildren().addAll(msg);

        ScrollPane scrollPaneWrap = new ScrollPane();
        scrollPaneWrap.setContent(grid);
        Scene scene = new Scene(scrollPaneWrap);
        window.setScene(scene);
        window.showAndWait(); // requires to be closed before returning to caller
    }
}
