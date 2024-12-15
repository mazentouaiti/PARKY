#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <gtk/gtk.h>


void
on_cancel_btn_edit_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_apply_btn_edit_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_delete_profile_btn_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_update_btn_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_btn_print_fact_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_togglebutton2_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_togglebutton1_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_button11_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_button12_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_button13_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_helpbutton1_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_cancelbutton1_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_okbutton1_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_cancelbutton2_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_okbutton2_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_cancelbutton3_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_okbutton3_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_user_interface_show                 (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_btn_show_profile_data_clicked       (GtkButton       *button,
                                        gpointer         user_data);

// In treeview_helpers.h or callbacks.h
void setup_tree_view(GtkTreeView *treeview);



#endif

void
on_cancelbutton4_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_okbutton4_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_cancelbutton4_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_okbutton4_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_reserv_btn_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_feedback_btn_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_btn_logout_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_btn_logout_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_radiomale_toggled                   (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_radiofemale_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_Addbtn_clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_signinbtn_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_sinupbtn_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_loginbtn_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_adminbtn_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_clientbtn_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_backaddtologin_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_backlogintowelcome_clicked          (GtkButton       *button,
                                        gpointer         user_data);




void
on_MGAdminRemovebutton_clicked         (GtkButton       *button1,
                                        gpointer         user_data);

void
on_MGAdminAddbutton_clicked            (GtkButton       *button2,
                                        gpointer         user_data);

void
on_MGAdminModifybutton_clicked         (GtkButton       *button3,
                                        gpointer         user_data);

void
on_AdminwindowParkConfig_show          (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_MGadminworkerUnassign_clicked       (GtkButton       *button,
                                        gpointer         user_data);

void
on_MGadminworkerAssign_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_MGCreataccbutton_clicked            (GtkButton       *button4,
                                        gpointer         user_data);

void
on_MGadminradiobuttonmale_toggled      (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_MGadminradiobuttonfemale_toggled    (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_MGAdminbuttonConnect_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_MGAdminbuttonNewAcc_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_MGAdminBacknewaccbutton_clicked     (GtkButton       *button,
                                        gpointer         user_data);

void
on_MGAdminLogoutbutton_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_MGAdminParkManagebutton_clicked     (GtkButton       *button,
                                        gpointer         user_data);

void
on_MGAdminBacktomainbutton_clicked     (GtkButton       *button,
                                        gpointer         user_data);

void
on_MGadminrefreshPARK_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_MGSearchPark_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_MGadminrefreshWorker_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_MGSearchWorker_clicked              (GtkButton       *button,
                                        gpointer         user_data);
void
on_MGadminBackorigine_clicked          (GtkButton       *button,
                                        gpointer         user_data);
