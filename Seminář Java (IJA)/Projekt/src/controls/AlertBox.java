package controls;
import javafx.scene.*;
import javafx.stage.*;
import javafx.scene.layout.*;
import javafx.scene.control.*;
import javafx.geometry.*;

/**
 * Class AlertBox implements basic pop-up window that informs user about changes or actions that he has made.
 * @author Michal Rein, Ondřej Motyčka
 */
public class AlertBox {

    /**
     * This method will create and show little pop-up window with given message and title. Window won't close until
     * user press 'Close' button.
     * @param title Title of window.
     * @param msg Message displayed in the window.
     */
    public static void display(String title, String msg){
        Stage window = new Stage();

        window.initModality(Modality.APPLICATION_MODAL); // block input event with other windows until this one is closed
        window.setTitle(title);
        window.setMinWidth(250);

        Label label = new Label();
        label.setText(msg);
        label.setPadding(new Insets(10));

        Button closeButton = new Button("Close");
        closeButton.setOnAction(e -> window.close());

        VBox layout = new VBox(10);
        layout.getChildren().addAll(label, closeButton);
        layout.setAlignment(Pos.CENTER);
        layout.setPadding(new Insets(10));

        Scene scene = new Scene(layout);
        window.setScene(scene);
        window.showAndWait(); // requires to be closed before returning to caller

    }
}
