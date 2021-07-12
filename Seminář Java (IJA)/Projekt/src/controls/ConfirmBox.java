package controls;
import javafx.scene.*;
import javafx.stage.*;
import javafx.scene.layout.*;
import javafx.scene.control.*;
import javafx.geometry.*;

/**
 * ConfirmBox implements simple pop-up window with message and two buttons with returning value True and False.
 * Serves to confirm action from user's side.
 * @author Michal Rein, Ondřej Motyčka
 */
public class ConfirmBox {

    static boolean answer;

    /**
     *
     * @param title Title of window.
     * @param msg Message (question) which user has to answer.
     * @return Boolean value, returns true when "yes" button is pressed, otherwise false.
     */
    public static boolean display(String title, String msg){
        Stage window = new Stage();
            window.initModality(Modality.APPLICATION_MODAL); // block input event with other windows until this one is closed
            window.setTitle(title);
            window.setMinWidth(250);

        Label label = new Label();
            label.setText(msg);
            GridPane.setConstraints(label, 1, 0);

        // create 2 buttons
        Button yesButton = new Button("Yes");
            yesButton.setOnAction(e -> {
                answer = true;
                window.close();
            });
            GridPane.setConstraints(yesButton, 0, 1);

        Button noButton = new Button("No");
            noButton.setOnAction(e -> {
                answer = false;
                window.close();
            });
            GridPane.setConstraints(noButton, 2, 1);

        yesButton.setAlignment(Pos.CENTER);

        GridPane grid = new GridPane();
            grid.setPadding(new Insets(10, 10, 10, 10)); // inset = object for padding
            grid.setVgap(8); // vertical gap of cells
            grid.setHgap(10); // horizontal gap of cells


        grid.getChildren().addAll(label, yesButton, noButton);
        grid.setAlignment(Pos.CENTER);

        Scene scene = new Scene(grid);
            window.setScene(scene);
            window.showAndWait(); // requires to be closed before returning to caller

        return answer;
    }
}
