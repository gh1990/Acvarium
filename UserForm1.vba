Option Explicit

' Event handler pentru schimbarea selectiei in ComboBox1
Private Sub ComboBox1_Change()
    ' OPTIMIZAT: Folosim noua functie comuna pentru actualizare controale
    ' Actualizam ComboBox1
    UpdateControlFromSelection "Liste", "Sector", "Brigadir", Me.ComboBox4, Me.ComboBox1, False
    ' Actualizam Label-urile conform ComboBox1
    UpdateControlFromSelection "Liste", "Sector", "MediaSalariala", Me.Label6, Me.ComboBox1, True
    UpdateControlFromSelection "Liste", "Sector", "NrMatricol", Me.Label2, Me.ComboBox1, True
    UpdateControlFromSelection "Liste", "Sector", "SefSector", Me.Label3, Me.ComboBox1, True
    UpdateControlFromSelection "Liste", "Sector", "Normator", Me.Label4, Me.ComboBox1, True
End Sub


Private Sub CommandButton10_Click()
    ' Salt direct la Page7
    Call JumpToLastPage(MultiPage1, CommandButton1, CommandButton2)
    ' Populam ComboBox5 cu coloanele din foaia "Obiecte"
    PopuleazaComboBoxCuColoane "Transport"
    
    ' Adaugam o eticheta in Tag pentru a sti ce fel de cautare facem
    Me.Tag = "CautareTransport"
    
    ' Populam ListBox2 cu datele din foaia Obiecte (FARA anteturi)
    ' Specificam latimile pentru cele doua coloane: 100;150
    PopuleazaListBoxCuDate "Transport", "45;45;170;30;40;20;30"
End Sub

Private Sub CommandButton11_Click()
    ' Buton de transfer date de la ListBox2 la TextBox-urile din Page1
    If Not ValidateCurrentPage(UserForm1, currentPage) Then
        ' Daca validarea esueaza, nu permitem avansarea
        Exit Sub
    End If
    
    Call AdaugaDateInListBox1(Me, "CautareTransport", "20;60;170;45;30;40;20;30")
End Sub

Private Sub CommandButton14_Click()
Call ReturnFromLastPage(MultiPage1, CommandButton1, CommandButton2)
End Sub

Private Sub CommandButton15_Click()
    
    If Not ValidateCurrentPage(UserForm1, currentPage) Then
        ' Daca validarea esueaza, nu permitem avansarea
        Exit Sub
    End If
    
    ' SAU pentru solutia 2 (transmitere directa a sursei)
     Call AdaugaDateInListBox1(Me, "CautareMateriale", "20;60;210;35;35;35")
End Sub

Private Sub CommandButton4_Click()
    ' Salt direct la Page7
    Call JumpToLastPage(MultiPage1, CommandButton1, CommandButton2)
    ' Populam ComboBox5 cu coloanele din foaia "Obiecte"
    PopuleazaComboBoxCuColoane "Obiect"
    
    ' Adaugam o eticheta in Tag pentru a sti ce fel de cautare facem
    Me.Tag = "CautareObiect"
    
    ' Populam ListBox2 cu datele din foaia Obiecte (FARA anteturi)
    ' Specificam latimile pentru cele doua coloane: 100;150
    PopuleazaListBoxCuDate "Obiect", "45;170"
End Sub

Private Sub CommandButton5_Click()
    ' Salt direct la Page7
    Call JumpToLastPage(MultiPage1, CommandButton1, CommandButton2)
    ' Populam ComboBox5 cu coloanele din foaia "Obiecte"
    PopuleazaComboBoxCuColoane "Norma"
    
    ' Adaugam o eticheta in Tag pentru a sti ce fel de cautare facem
    Me.Tag = "CautareNorma"
    
    ' Populam ListBox2 cu datele din foaia Obiecte (FARA anteturi)
    ' Specificam latimile pentru cele doua coloane: 100;150
    PopuleazaListBoxCuDate "Norma", "45;295;25;30"
End Sub

Private Sub CommandButton6_Click()
    ' Salt direct la Page7
    Call JumpToLastPage(MultiPage1, CommandButton1, CommandButton2)
    ' Populam ComboBox5 cu coloanele din foaia "Obiecte"
    PopuleazaComboBoxCuColoane "Materiale"
    
    ' Adaugam o eticheta in Tag pentru a sti ce fel de cautare facem
    Me.Tag = "CautareMateriale"
    
    ' Populam ListBox2 cu datele din foaia Obiecte (FARA anteturi)
    ' Specificam latimile pentru cele doua coloane: 100;150
    PopuleazaListBoxCuDate "Materiale", "60;210;35;35;35"
End Sub

Private Sub CommandButton7_Click()
    ' Salt direct la Page7
    Call JumpToLastPage(MultiPage1, CommandButton1, CommandButton2)
    ' Populam ComboBox5 cu coloanele din foaia "Obiecte"
    PopuleazaComboBoxCuColoane "Utilaj"
    
    ' Adaugam o eticheta in Tag pentru a sti ce fel de cautare facem
    Me.Tag = "CautareUtilaj"
    
    ' Populam ListBox2 cu datele din foaia Obiecte (FARA anteturi)
    ' Specificam latimile pentru cele doua coloane: 100;150
    PopuleazaListBoxCuDate "Utilaj", "100;35;35"
End Sub


Private Sub TextBox26_Change()
    ' Cautare in timp real cand se modifica textul
    Dim Criteriu As String
    Dim Coloana As String
    Dim sheetName As String
    
    ' Obtinem criteriul de cautare
    Criteriu = TextBox26.Text
    
    ' Verificam daca avem o coloana selectata
    If ComboBox6.ListIndex = -1 Then
        ' Nu avem coloana selectata, nu putem cauta
        Exit Sub
    End If
    
    ' Obtinem coloana selectata
    Coloana = ComboBox6.Text
    
    ' Determinam foaia din Tag
    If InStr(Me.Tag, "Cautare") > 0 Then
        sheetName = Replace(Me.Tag, "Cautare", "")
    Else
        ' Nu avem foaie specificata, nu putem cauta
        Exit Sub
    End If
    
    ' Apelam functia de filtrare in timp real
    FiltreazaListBoxInTimpReal sheetName, Criteriu, Coloana

End Sub


Private Sub UserForm_Initialize()
    Dim currentMonth As String
    ' Initializarea formei
    InitializeazaFormular UserForm1
    'Alege Sectorul
    Call PopulateComboBoxFromColumn_BDFisaPV(Me.ComboBox1, "Liste", "Sector", True)
    'Alege Brigada
    Call PopulateComboBoxFromColumn_BDFisaPV(Me.ComboBox4, "Liste", "Brigadir", True)
    'Alege Luna
    Call PopulateComboBoxFromColumn_BDFisaPV(Me.ComboBox2, "Liste", "Luna", True)
    
    Call PopulateComboBoxFromColumn_BDFisaPV(Me.ComboBox5, "Liste", "Functia", True)
    
    'Obtinem luna curenta in romana
    currentMonth = GetCurrentMonthInRomanian()
    ' Selectam luna curenta in ComboBox2
    SelectItemInComboBox Me.ComboBox2, currentMonth
    
    Call InitializeMultiPage(Me, MultiPage1, CommandButton1, CommandButton2)
    
End Sub

Private Sub CommandButton2_Click()
    ' Butonul NEXT
    Call GoToNextPage(Me, MultiPage1, CommandButton1, CommandButton2)
End Sub

Private Sub CommandButton1_Click()
    ' Butonul BACK
    Call GoToPrevPage(MultiPage1, CommandButton1, CommandButton2)
End Sub

Private Sub MultiPage1_Change()
    ' Previne schimbarea manuala a paginilor prin click pe tab
    Call PreventManualPageChange(MultiPage1)
End Sub
Private Sub ComboBox6_Change()
    ' Curata TextBox9 cand se schimba coloana de cautare
    TextBox26.Text = ""
End Sub
Private Sub CommandButton13_Click()
    ' Revenire de pe Page7 la pagina anterioara
    
    ' Buton de transfer date de la ListBox2 la TextBox-urile din Page1
    ' Verificam daca exista o linie selectata
    If ListBox2.ListIndex = -1 Then
        MsgBox "Selectati o linie din lista!", vbExclamation
        Exit Sub
    End If
    Call ReturnFromLastPage(MultiPage1, CommandButton1, CommandButton2)
    ' Determinam sursa datelor din tag-ul formularului
    Dim sursaDate As String
    sursaDate = Me.Tag
    
    ' Variabile pentru stocarea datelor
    Dim valoare1 As String
    Dim valoare2 As String
    Dim valoare3 As String
    Dim valoare4 As String
    Dim valoare5 As String
    Dim valoare6 As String
    Dim valoare7 As String
    ' Tratam diferit in functie de sursa datelor
    Select Case sursaDate
        Case "CautareObiect"
            ' Extragem datele specifice foii Obiect
            valoare1 = ListBox2.List(ListBox2.ListIndex, 1) ' NumeObiect (coloana 1)
            valoare2 = ListBox2.List(ListBox2.ListIndex, 0) ' NrInventar (coloana 0)
            
            ' Transferam datele in TextBox-urile din Page1
            TextBox1.Text = valoare1 ' NumeObiect
            TextBox2.Text = valoare2 ' NrInventar
                       
        Case "CautareNorma"
            ' Extragem datele specifice foii Norma
            ' Ajustati indicii in functie de structura reala a datelor din foaia Norma
            valoare1 = ListBox2.List(ListBox2.ListIndex, 0) ' Prima coloana relevanta
            valoare2 = ListBox2.List(ListBox2.ListIndex, 1) ' A doua coloana relevanta
            valoare3 = ListBox2.List(ListBox2.ListIndex, 2) ' A treia coloana relevanta (daca exista)
            valoare4 = ListBox2.List(ListBox2.ListIndex, 3) ' A treia coloana relevanta (daca exista)
            ' Transferam datele in TextBox-urile din Page1 (ajustati conform nevoilor)
            TextBox4.Text = valoare1
            TextBox5.Text = valoare2
            TextBox6.Text = valoare3
            TextBox8.Text = valoare4
            ' TextBox4.Text = valoare3 ' Daca aveti si alte campuri in care transferati date
        Case "CautareMateriale"
            ' Extragem datele specifice foii Norma
            ' Ajustati indicii in functie de structura reala a datelor din foaia Norma
            valoare1 = ListBox2.List(ListBox2.ListIndex, 0) ' Prima coloana relevanta
            valoare2 = ListBox2.List(ListBox2.ListIndex, 1) ' A doua coloana relevanta
            valoare3 = ListBox2.List(ListBox2.ListIndex, 2) ' A treia coloana relevanta (daca exista)
            valoare4 = ListBox2.List(ListBox2.ListIndex, 3) ' A treia coloana relevanta (daca exista)
            valoare5 = ListBox2.List(ListBox2.ListIndex, 4) ' A treia coloana relevanta (daca exista)
            ' Transferam datele in TextBox-urile din Page1 (ajustati conform nevoilor)
            TextBox11.Text = valoare1
            TextBox12.Text = valoare2
            TextBox13.Text = valoare3
            TextBox15.Text = valoare5
            ' TextBox4.Text = valoare3 ' Daca aveti si alte campuri in care transferati date
        Case "CautareUtilaj"
            ' Extragem datele specifice foii Norma
            ' Ajustati indicii in functie de structura reala a datelor din foaia Norma
            valoare1 = ListBox2.List(ListBox2.ListIndex, 0) ' Prima coloana relevanta
            valoare2 = ListBox2.List(ListBox2.ListIndex, 1) ' A doua coloana relevanta
            valoare3 = ListBox2.List(ListBox2.ListIndex, 2) ' A treia coloana relevanta (daca exista)
            ' Transferam datele in TextBox-urile din Page1 (ajustati conform nevoilor)
            TextBox16.Text = valoare1
            TextBox17.Text = valoare2
            TextBox18.Text = valoare3
        Case "CautareTransport"
            ' Extragem datele specifice foii Norma
            ' Ajustati indicii in functie de structura reala a datelor din foaia Norma
            valoare1 = ListBox2.List(ListBox2.ListIndex, 0) ' Prima coloana relevanta
            valoare2 = ListBox2.List(ListBox2.ListIndex, 1) ' A doua coloana relevanta
            valoare3 = ListBox2.List(ListBox2.ListIndex, 2) ' A treia coloana relevanta (daca exista)
            valoare4 = ListBox2.List(ListBox2.ListIndex, 3) ' A treia coloana relevanta (daca exista)
            valoare5 = ListBox2.List(ListBox2.ListIndex, 4) ' A treia coloana relevanta (daca exista)
            valoare6 = ListBox2.List(ListBox2.ListIndex, 5) ' A treia coloana relevanta (daca exista)
            valoare7 = ListBox2.List(ListBox2.ListIndex, 6) ' A treia coloana relevanta (daca exista)
            ' Transferam datele in TextBox-urile din Page1 (ajustati conform nevoilor)
            TextBox19.Text = valoare1
            TextBox20.Text = valoare3
            TextBox21.Text = valoare2
            TextBox22.Text = valoare4
            TextBox23.Text = valoare5
            TextBox24.Text = valoare6
            TextBox25.Text = valoare7
            
        Case Else
            ' Caz pentru alte surse de date (daca va fi cazul)
            MsgBox "Sursa de date necunoscuta!", vbExclamation
            Exit Sub
    End Select
    
End Sub
Private Sub CommandButton9_Click()
    ' Buton de revenire la Page1 fara transfer de date
    
    ' Dezactivam Page2 si activam Page1
    MultiPage1.Pages("Page2").Enabled = False
    MultiPage1.Pages("Page1").Enabled = True
    
    ' Trecem la Page1 (index 0)
    MultiPage1.Value = 0
End Sub

Private Sub CommandButton8_Click()
    ' Buton de transfer date de la ListBox2 la TextBox-urile din Page1
    If Not ValidateCurrentPage(UserForm1, currentPage) Then
        ' Daca validarea esueaza, nu permitem avansarea
        Exit Sub
    End If
    
    Call AdaugaDateInListBox1(Me, "CautareUtilaj", "20;150;35;35")
End Sub
