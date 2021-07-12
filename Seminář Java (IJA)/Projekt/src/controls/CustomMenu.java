package controls;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.control.*;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import java.io.File;


/**
 * CustomMenu class represents menu in the top part of an application.
 * @author Michal Rein, Ondřej Motyčka
 */
public class CustomMenu {

    /** Menu bar handle. */
    private MenuBar menuBarHandle;

    /**
     * Constructor of custom menu bar.
     * @param menuBar Menu bar handle.
     */
    public CustomMenu(MenuBar menuBar){
        menuBarHandle = menuBar;
        setMenu();
    }

    /**
     * Create menu, add options and add all elements to the menu handle.
     */
    private void setMenu(){
        //Menu menuFile = new Menu("File");
            /*MenuItem itemLoad  = new MenuItem("Load map");
                itemLoad.setOnAction(new EventHandler<ActionEvent>() {
                    @Override
                    public void handle(ActionEvent event) {

                        Stage window = new Stage();
                        window.setTitle("IJA-Project Menu");

                        // get JSON map representation from user
                        final FileChooser fileChooser = new FileChooser();
                        File fp = fileChooser.showOpenDialog(window);

                        if (fp == null){
                            window.close();
                        } else {
                            String[] argument = {fp.getPath()};
                            Main newAppInstance = new Main();
                            newAppInstance.newAppWindow(argument);
                            window.close();
                        }
                    }
                }); */
            /*MenuItem itemHelp  = new MenuItem("Help");
                itemHelp.setOnAction(new EventHandler<ActionEvent>() {
                    @Override
                    public void handle(ActionEvent event) {
                        ShowBox.displayHelp();
                    }
                });
            menuFile.getItems().addAll(itemLoad, itemHelp);*/

        /*Menu menuEdit = new Menu("Edit");
            MenuItem itemSetStop = new MenuItem("Edit Stop");
            MenuItem itemSetStreet = new MenuItem("Edit Street");
            MenuItem itemSetLine = new MenuItem("Edit Line");
            menuEdit.getItems().addAll(itemSetStop, itemSetStreet, itemSetLine);
        Menu menuView = new Menu("View");*/

        Menu menuHelp = new Menu("Help");
            MenuItem itemHelp  = new MenuItem("Show help");
            itemHelp.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                ShowBox.displayHelp();
            }
        });
        menuHelp.getItems().addAll(itemHelp);
        menuBarHandle.getMenus().addAll(menuHelp);
    }
}
