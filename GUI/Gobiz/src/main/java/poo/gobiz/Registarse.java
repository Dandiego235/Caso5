/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/GUIForms/JFrame.java to edit this template
 */
package poo.gobiz;

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import javax.swing.JOptionPane;

/**
 *
 * @author dandi
 */
public class Registarse extends javax.swing.JFrame {
      private String nickname;
      private String password;
      private String confirm;
      private String offer;
      private String demand;
      /**
       * Creates new form Registarse
       */
      public Registarse() {
            initComponents();
            this.setLocationRelativeTo(null);
            OfertaTXT.setLineWrap(true);
            OfertaTXT.setWrapStyleWord(true);
            DemandaTXT.setLineWrap(true);
            DemandaTXT.setWrapStyleWord(true);
      }

      /**
       * This method is called from within the constructor to initialize the
       * form. WARNING: Do NOT modify this code. The content of this method is
       * always regenerated by the Form Editor.
       */
      @SuppressWarnings("unchecked")
      // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
      private void initComponents() {

            jLabel1 = new javax.swing.JLabel();
            jLabel2 = new javax.swing.JLabel();
            jLabel3 = new javax.swing.JLabel();
            jLabel4 = new javax.swing.JLabel();
            NicknameTXT = new javax.swing.JTextField();
            ContraseñaTXT = new javax.swing.JPasswordField();
            ConfirmTXT = new javax.swing.JPasswordField();
            jLabel5 = new javax.swing.JLabel();
            jScrollPane1 = new javax.swing.JScrollPane();
            OfertaTXT = new javax.swing.JTextArea();
            jLabel6 = new javax.swing.JLabel();
            jScrollPane2 = new javax.swing.JScrollPane();
            DemandaTXT = new javax.swing.JTextArea();
            jButton1 = new javax.swing.JButton();
            jButton2 = new javax.swing.JButton();

            setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);

            jLabel1.setFont(new java.awt.Font("Yu Gothic UI", 0, 14)); // NOI18N
            jLabel1.setText("Registarse");

            jLabel2.setFont(new java.awt.Font("Yu Gothic UI", 0, 12)); // NOI18N
            jLabel2.setText("Nickname");

            jLabel3.setFont(new java.awt.Font("Yu Gothic UI", 0, 12)); // NOI18N
            jLabel3.setText("Contraseña");

            jLabel4.setFont(new java.awt.Font("Yu Gothic UI", 0, 12)); // NOI18N
            jLabel4.setText("Confirmar Contraseña");

            NicknameTXT.setFont(new java.awt.Font("Yu Gothic UI", 0, 12)); // NOI18N
            NicknameTXT.addActionListener(new java.awt.event.ActionListener() {
                  public void actionPerformed(java.awt.event.ActionEvent evt) {
                        NicknameTXTActionPerformed(evt);
                  }
            });

            ContraseñaTXT.setFont(new java.awt.Font("Yu Gothic UI", 0, 12)); // NOI18N

            ConfirmTXT.setFont(new java.awt.Font("Yu Gothic UI", 0, 12)); // NOI18N

            jLabel5.setFont(new java.awt.Font("Yu Gothic UI", 0, 12)); // NOI18N
            jLabel5.setText("Oferta");

            OfertaTXT.setColumns(20);
            OfertaTXT.setFont(new java.awt.Font("Yu Gothic UI", 0, 12)); // NOI18N
            OfertaTXT.setRows(5);
            jScrollPane1.setViewportView(OfertaTXT);

            jLabel6.setFont(new java.awt.Font("Yu Gothic UI", 0, 12)); // NOI18N
            jLabel6.setText("Demanda");

            DemandaTXT.setColumns(20);
            DemandaTXT.setFont(new java.awt.Font("Yu Gothic UI", 0, 12)); // NOI18N
            DemandaTXT.setRows(5);
            jScrollPane2.setViewportView(DemandaTXT);

            jButton1.setFont(new java.awt.Font("Yu Gothic UI", 0, 14)); // NOI18N
            jButton1.setText("Registrar");
            jButton1.addActionListener(new java.awt.event.ActionListener() {
                  public void actionPerformed(java.awt.event.ActionEvent evt) {
                        jButton1ActionPerformed(evt);
                  }
            });

            jButton2.setFont(new java.awt.Font("Yu Gothic UI", 0, 14)); // NOI18N
            jButton2.setText("Cancelar");
            jButton2.addActionListener(new java.awt.event.ActionListener() {
                  public void actionPerformed(java.awt.event.ActionEvent evt) {
                        jButton2ActionPerformed(evt);
                  }
            });

            javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
            getContentPane().setLayout(layout);
            layout.setHorizontalGroup(
                  layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                  .addGroup(layout.createSequentialGroup()
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                              .addGroup(layout.createSequentialGroup()
                                    .addGap(168, 168, 168)
                                    .addComponent(jLabel1))
                              .addGroup(layout.createSequentialGroup()
                                    .addContainerGap()
                                    .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                          .addComponent(jLabel6)
                                          .addComponent(jLabel5))
                                    .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                    .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                                          .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 239, Short.MAX_VALUE)
                                          .addComponent(jScrollPane2))
                                    .addGap(18, 18, 18)
                                    .addComponent(jButton2))
                              .addGroup(layout.createSequentialGroup()
                                    .addContainerGap()
                                    .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                          .addComponent(jLabel2)
                                          .addComponent(jLabel3)
                                          .addComponent(jLabel4))
                                    .addGap(22, 22, 22)
                                    .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                                          .addComponent(NicknameTXT)
                                          .addComponent(ContraseñaTXT)
                                          .addComponent(ConfirmTXT, javax.swing.GroupLayout.DEFAULT_SIZE, 157, Short.MAX_VALUE))
                                    .addGap(29, 29, 29)
                                    .addComponent(jButton1)))
                        .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
            );
            layout.setVerticalGroup(
                  layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                  .addGroup(layout.createSequentialGroup()
                        .addComponent(jLabel1)
                        .addGap(18, 18, 18)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                              .addComponent(jLabel2)
                              .addComponent(NicknameTXT, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addGap(27, 27, 27)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                              .addComponent(jLabel3)
                              .addComponent(ContraseñaTXT, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addGap(32, 32, 32)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                              .addComponent(jLabel4)
                              .addComponent(ConfirmTXT, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                              .addComponent(jButton1))
                        .addGap(18, 18, 18)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                              .addComponent(jLabel5)
                              .addComponent(jScrollPane1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                              .addComponent(jButton2))
                        .addGap(21, 21, 21)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                              .addComponent(jLabel6)
                              .addComponent(jScrollPane2, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addGap(0, 10, Short.MAX_VALUE))
            );

            pack();
      }// </editor-fold>//GEN-END:initComponents

      private void NicknameTXTActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_NicknameTXTActionPerformed
            // TODO add your handling code here:
            
            
            
      }//GEN-LAST:event_NicknameTXTActionPerformed

      private void jButton1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton1ActionPerformed
            // TODO add your handling code here:
            if (NicknameTXT.getText().equals("") || NicknameTXT.getText().length() < 10 || NicknameTXT.getText().length() > 32){
                  JOptionPane.showMessageDialog(this, "EL NICKNAME ES INVÁLIDO", 
                    "Error", JOptionPane.ERROR_MESSAGE);
                  return;
            }
            
            // buscar si el nickname ya está registrado
            nickname = NicknameTXT.getText();
            if (ContraseñaTXT.getText().equals("") || ContraseñaTXT.getText().length() > 20){
                  JOptionPane.showMessageDialog(this, "LA CONTRASEÑA ES INVÁLIDA", 
                    "Error", JOptionPane.ERROR_MESSAGE);
                  return;
            }
            password = ContraseñaTXT.getText();
            
            if (!ConfirmTXT.getText().equals(password)){
                  JOptionPane.showMessageDialog(this, "LAS DOS CONTRASEÑAS NO SON IGUALES", 
                    "Error", JOptionPane.ERROR_MESSAGE);
                  return;
            }
            if (OfertaTXT.getText().length() > 250 && !OfertaTXT.getText().equals("")){
                  JOptionPane.showMessageDialog(this, "LA OFERTA ES INVÁLIDA", 
                    "Error", JOptionPane.ERROR_MESSAGE);
                  return;
            }
            if (DemandaTXT.getText().length() > 250 && !DemandaTXT.getText().equals("")){
                  JOptionPane.showMessageDialog(this, "LA DEMANDA ES INVÁLIDA", 
                    "Error", JOptionPane.ERROR_MESSAGE);
                  return;
            }
             DateTimeFormatter dtf = DateTimeFormatter.ofPattern("yyyy/MM/dd");  
             LocalDateTime now = LocalDateTime.now();  
            System.out.println(dtf.format(now)); 
            JOptionPane.showMessageDialog(this, "Se registró el usuario. Se buscarán los matches de acuerdo con lo ingresado."); 
      }//GEN-LAST:event_jButton1ActionPerformed

      private void jButton2ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton2ActionPerformed
            // TODO add your handling code here:
            dispose();
            MenuPrincipal window = new MenuPrincipal();
            window.setVisible(true);
      }//GEN-LAST:event_jButton2ActionPerformed

      /**
       * @param args the command line arguments
       */
      public static void main(String args[]) {
            /* Set the Nimbus look and feel */
            //<editor-fold defaultstate="collapsed" desc=" Look and feel setting code (optional) ">
            /* If Nimbus (introduced in Java SE 6) is not available, stay with the default look and feel.
         * For details see http://download.oracle.com/javase/tutorial/uiswing/lookandfeel/plaf.html 
             */
            try {
                  for (javax.swing.UIManager.LookAndFeelInfo info : javax.swing.UIManager.getInstalledLookAndFeels()) {
                        if ("Nimbus".equals(info.getName())) {
                              javax.swing.UIManager.setLookAndFeel(info.getClassName());
                              break;
                        }
                  }
            } catch (ClassNotFoundException ex) {
                  java.util.logging.Logger.getLogger(Registarse.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
            } catch (InstantiationException ex) {
                  java.util.logging.Logger.getLogger(Registarse.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
            } catch (IllegalAccessException ex) {
                  java.util.logging.Logger.getLogger(Registarse.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
            } catch (javax.swing.UnsupportedLookAndFeelException ex) {
                  java.util.logging.Logger.getLogger(Registarse.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
            }
            //</editor-fold>

            /* Create and display the form */
            java.awt.EventQueue.invokeLater(new Runnable() {
                  public void run() {
                        new Registarse().setVisible(true);
                  }
            });
      }

      // Variables declaration - do not modify//GEN-BEGIN:variables
      private javax.swing.JPasswordField ConfirmTXT;
      private javax.swing.JPasswordField ContraseñaTXT;
      private javax.swing.JTextArea DemandaTXT;
      private javax.swing.JTextField NicknameTXT;
      private javax.swing.JTextArea OfertaTXT;
      private javax.swing.JButton jButton1;
      private javax.swing.JButton jButton2;
      private javax.swing.JLabel jLabel1;
      private javax.swing.JLabel jLabel2;
      private javax.swing.JLabel jLabel3;
      private javax.swing.JLabel jLabel4;
      private javax.swing.JLabel jLabel5;
      private javax.swing.JLabel jLabel6;
      private javax.swing.JScrollPane jScrollPane1;
      private javax.swing.JScrollPane jScrollPane2;
      // End of variables declaration//GEN-END:variables
}
