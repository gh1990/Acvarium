Option Explicit

' Variabila publica pentru a stoca indexul paginii active curent
Public currentPage As Integer
' Variabila publica pentru a stoca pagina anterioara (de unde s-a facut salt la Page7)
Public previousPage As Integer
' Modul standard pentru variabila globala
Public wbBDFisaPV As Workbook

' Functie pentru initializarea formularului - seteaza starea initiala
Public Sub InitializeazaFormular(frm As UserForm1)
    frm.MultiPage1.Value = 0
    frm.MultiPage1.Enabled = False
End Sub

' Functie pentru a actualiza starea butoanelor de navigare
Private Sub UpdateButtons()
    CommandButton1.Enabled = (currentPage > 0)
    CommandButton2.Enabled = (currentPage < MultiPage1.Pages.Count - 1)
End Sub

' Functie care returneaza numele lunii curente in romana
Public Function GetCurrentMonthInRomanian() As String
    Dim monthNames(1 To 12) As String
    Dim currentMonth As Integer
    
    monthNames(1) = "Ianuarie"
    monthNames(2) = "Februarie"
    monthNames(3) = "Martie"
    monthNames(4) = "Aprilie"
    monthNames(5) = "Mai"
    monthNames(6) = "Iunie"
    monthNames(7) = "Iulie"
    monthNames(8) = "August"
    monthNames(9) = "Septembrie"
    monthNames(10) = "Octombrie"
    monthNames(11) = "Noiembrie"
    monthNames(12) = "Decembrie"
    
    currentMonth = Month(Date)
    
    GetCurrentMonthInRomanian = monthNames(currentMonth)
End Function

' Functie care selecteaza un element specific in ComboBox
Public Function SelectItemInComboBox(ByRef cboTarget As MSForms.comboBox, ByVal itemText As String) As Boolean
    Dim i As Long
    
    SelectItemInComboBox = False
    
    If cboTarget Is Nothing Then
        Exit Function
    End If
    
    For i = 0 To cboTarget.ListCount - 1
        If cboTarget.List(i) = itemText Then
            cboTarget.ListIndex = i
            SelectItemInComboBox = True
            Exit Function
        End If
    Next i
    
    If cboTarget.ListCount > 0 Then
        cboTarget.ListIndex = 0
        SelectItemInComboBox = True
    End If
End Function

' Functie ajutatoare pentru a gasi indexul coloanei
Private Function FindColumnIndex(ws As Worksheet, columnName As String) As Integer
    Dim i As Integer
    FindColumnIndex = 0
    
    For i = 1 To ws.UsedRange.Columns.Count
        If ws.Cells(1, i).Value = columnName Then
            FindColumnIndex = i
            Exit Function
        End If
    Next i
End Function

' Functie ajutatoare pentru a gasi randul cu valoarea cautata
Private Function FindRowWithValue(ws As Worksheet, colIndex As Integer, searchValue As String, lastRow As Long) As Long
    Dim i As Long
    FindRowWithValue = 0
    
    For i = 2 To lastRow ' Incepem de la 2 pentru a sari peste antet
        If ws.Cells(i, colIndex).Value = searchValue Then
            FindRowWithValue = i
            Exit Function
        End If
    Next i
End Function

' OPTIMIZAT: Functie pentru popularea ComboBox din workbook-ul deja deschis
Public Function PopulateComboBoxFromColumn_BDFisaPV( _
    ByRef cboTarget As MSForms.comboBox, _
    ByVal sheetName As String, _
    ByVal columnName As String, _
    Optional ByVal selectFirstItem As Boolean = True) As Boolean

    On Error GoTo ErrorHandler

    Dim ws As Worksheet
    Dim dataRange As Range
    Dim dataArray As Variant
    Dim dict As Object ' Dictionary pentru a evita duplicatele
    Dim i As Long, j As Long
    Dim columnIndex As Integer
    Dim lastRow As Long
    Dim var As Variant

    If cboTarget Is Nothing Then
        MsgBox "Nu a fost specificat ComboBox-ul tinta!", vbExclamation, "Eroare"
        Exit Function
    End If

    'If wbBDFisaPV Is Nothing Then
    '    MsgBox "Baza de date nu este deschisa!", vbCritical, "Eroare"
    '    Exit Function
    'End If

    On Error Resume Next
    Set ws = wbBDFisaPV.Worksheets(sheetName)
    On Error GoTo ErrorHandler

    If ws Is Nothing Then
        MsgBox "Foaia '" & sheetName & "' nu exista in baza de date!", vbExclamation, "Eroare"
        Exit Function
    End If

    ' Gasirea coloanei
    columnIndex = FindColumnIndex(ws, columnName)

    If columnIndex = 0 Then
        MsgBox "Coloana '" & columnName & "' nu exista in foaia '" & sheetName & "'!", vbExclamation, "Eroare"
        Exit Function
    End If

    lastRow = ws.Cells(ws.Rows.Count, columnIndex).End(xlUp).Row

    If lastRow <= 1 Then
        MsgBox "Nu exista date in coloana '" & columnName & "'!", vbExclamation, "Informatie"
        Exit Function
    End If

    ' Citeste toate datele intr-un array - mult mai rapid
    dataArray = ws.Range(ws.Cells(2, columnIndex), ws.Cells(lastRow, columnIndex)).Value
    
    ' Creare dictionary pentru eliminarea duplicatelor
    Set dict = CreateObject("Scripting.Dictionary")
    
    ' Goleste ComboBox
    cboTarget.Clear
    
    ' Adauga valori unice in dictionar
    If IsArray(dataArray) Then
        ' Daca sunt mai multe randuri
        For i = 1 To UBound(dataArray, 1)
            If Not isEmpty(dataArray(i, 1)) And Not dict.exists(CStr(dataArray(i, 1))) Then
                dict.Add CStr(dataArray(i, 1)), Nothing
            End If
        Next i
    Else
        ' Daca e doar un rand
        If Not isEmpty(dataArray) And Not dict.exists(CStr(dataArray)) Then
            dict.Add CStr(dataArray), Nothing
        End If
    End If
    
    ' Adauga valorile din dictionar in ComboBox
    For Each var In dict.keys
        cboTarget.AddItem var
    Next
    
    ' Selecteaza primul element daca exista si e necesar
    If selectFirstItem And cboTarget.ListCount > 0 Then
        cboTarget.ListIndex = 0
    End If

    PopulateComboBoxFromColumn_BDFisaPV = True
    Exit Function

ErrorHandler:
    MsgBox "A aparut o eroare la popularea ComboBox-ului: " & Err.Description, vbCritical, "Eroare"
    PopulateComboBoxFromColumn_BDFisaPV = False
End Function

' OPTIMIZAT: Functie comuna pentru actualizarea controalelor (Label sau ComboBox)
Public Function UpdateControlFromSelection(ByVal sheetName As String, _
                                          ByVal sourceColumnName As String, _
                                          ByVal targetColumnName As String, _
                                          ByRef targetControl As Object, _
                                          ByRef sourceComboRef As MSForms.comboBox, _
                                          Optional ByVal isLabel As Boolean = False) As Boolean
    On Error GoTo ErrorHandler
    
    Dim ws As Worksheet
    Dim sourceColIndex As Integer
    Dim targetColIndex As Integer
    Dim selectedValue As String
    Dim rowFound As Long
    Dim targetValue As Variant
    Dim lastRow As Long
    Dim i As Long
    
    UpdateControlFromSelection = False
    
    ' Verificam referintele
    If targetControl Is Nothing Or sourceComboRef Is Nothing Then
        MsgBox "Referintele la controale nu sunt valide!", vbExclamation, "Eroare"
        Exit Function
    End If
    
    ' Verificam daca e selectat un element
    If sourceComboRef.ListIndex = -1 Then
        If isLabel Then targetControl.Caption = ""
        Exit Function
    End If
    
    selectedValue = sourceComboRef.Value
    
    ' Deschiderea foii
    On Error Resume Next
    Set ws = wbBDFisaPV.Worksheets(sheetName)
    On Error GoTo ErrorHandler
    
     Verificare foaie
    If ws Is Nothing Then
        MsgBox "Foaia '" & sheetName & "' nu exista!", vbExclamation, "Eroare"
        Exit Function
    End If
    
    ' Gasirea coloanelor
    sourceColIndex = FindColumnIndex(ws, sourceColumnName)
    targetColIndex = FindColumnIndex(ws, targetColumnName)
    
    ' Verificare coloane
    If sourceColIndex = 0 Then
        MsgBox "Coloana '" & sourceColumnName & "' nu exista!", vbExclamation, "Eroare"
        Exit Function
    End If
    
    If targetColIndex = 0 Then
        MsgBox "Coloana '" & targetColumnName & "' nu exista!", vbExclamation, "Eroare"
        Exit Function
    End If
    
    ' Determinam ultima linie cu date
    lastRow = ws.Cells(ws.Rows.Count, sourceColIndex).End(xlUp).Row
    
    ' Cautam valoarea
    rowFound = FindRowWithValue(ws, sourceColIndex, selectedValue, lastRow)
    
    ' Daca am gasit valoarea
    If rowFound > 0 Then
        targetValue = ws.Cells(rowFound, targetColIndex).Value
        
        ' Actualizam controlul
        If isLabel Then
            targetControl.Caption = targetValue
            UpdateControlFromSelection = True
        Else
            ' Cautam in ComboBox si selectam
            For i = 0 To targetControl.ListCount - 1
                If targetControl.List(i) = targetValue Then
                    targetControl.ListIndex = i
                    UpdateControlFromSelection = True
                    Exit Function
                End If
            Next i
        End If
    Else
        If isLabel Then targetControl.Caption = ""
    End If
    
    Exit Function
    
ErrorHandler:
    MsgBox "A aparut o eroare: " & Err.Description, vbCritical, "Eroare"
    UpdateControlFromSelection = False
End Function

' Functie pentru initializarea MultiPage
Public Sub InitializeMultiPage(frm As UserForm, MP As MSForms.MultiPage, btnBack As MSForms.CommandButton, btnNext As MSForms.CommandButton)
    MP.Enabled = False
    currentPage = 0
    
    btnBack.Enabled = False
    btnNext.Enabled = True
    
    Dim i As Integer
    For i = 0 To MP.Pages.Count - 1
        MP.Pages(i).Enabled = False
    Next i
End Sub

' Functie pentru avansarea la pagina urmatoare
Public Sub GoToNextPage(frm As MSForms.UserForm, MP As MSForms.MultiPage, btnBack As MSForms.CommandButton, btnNext As MSForms.CommandButton)
    If Not MP.Enabled Then
        MP.Enabled = True
        ActivatePageByIndex MP, btnBack, btnNext, 0
        Exit Sub
    End If
    Select Case currentPage
        Case 0
            If Not ValidateCurrentPage(UserForm1, currentPage) Then
            Exit Sub
            End If
            ActivatePageByIndex MP, btnBack, btnNext, 1
            Call AdaugaDateInListBox1(frm, "CautareObiect", "45;50;100")
        Case 1
            If Not ValidateCurrentPage(UserForm1, currentPage) Then
            Exit Sub
            End If
            ActivatePageByIndex MP, btnBack, btnNext, 2
            Call AdaugaDateInListBox1(frm, "CautareLucrare", "50")
        Case 2
            If Not ValidateCurrentPage(UserForm1, currentPage) Then
            Exit Sub
            End If
            ActivatePageByIndex MP, btnBack, btnNext, 3
            Call AdaugaDateInListBox1(frm, "CautareNorma", "45;45;45;45")
        Case 3
            ActivatePageByIndex MP, btnBack, btnNext, 4
        Case 4
            ActivatePageByIndex MP, btnBack, btnNext, 5
        Case 5
            ActivatePageByIndex MP, btnBack, btnNext, 6
            btnNext.Enabled = False
    End Select
End Sub

' Functie pentru revenirea la pagina anterioara
Public Sub GoToPrevPage(MP As MSForms.MultiPage, btnBack As MSForms.CommandButton, btnNext As MSForms.CommandButton)
    Select Case currentPage
        Case 1
            ActivatePageByIndex MP, btnBack, btnNext, 0
        Case 2
            ActivatePageByIndex MP, btnBack, btnNext, 1
        Case 3
            ActivatePageByIndex MP, btnBack, btnNext, 2
        Case 4
            ActivatePageByIndex MP, btnBack, btnNext, 3
        Case 5
            ActivatePageByIndex MP, btnBack, btnNext, 4
        Case 6
            ActivatePageByIndex MP, btnBack, btnNext, 5
        Case 0
            btnBack.Enabled = False
    End Select
End Sub

' Functie pentru activarea unei pagini specifice
Public Sub ActivatePageByIndex(MP As MSForms.MultiPage, btnBack As MSForms.CommandButton, btnNext As MSForms.CommandButton, pageIndex As Integer)
    Dim i As Integer
    For i = 0 To MP.Pages.Count - 1
        MP.Pages(i).Enabled = False
    Next i
    
    MP.Pages(pageIndex).Enabled = True
    
    MP.Value = pageIndex
    
    currentPage = pageIndex
    
    If pageIndex = 7 Then
        btnBack.Enabled = False
        btnNext.Enabled = False
    Else
        UpdateButtonStates btnBack, btnNext, 7
    End If
End Sub

' Functie pentru actualizarea starii butoanelor
Public Sub UpdateButtonStates(btnBack As MSForms.CommandButton, btnNext As MSForms.CommandButton, lastPageIndex As Integer)
    btnBack.Enabled = (currentPage > 5)
    
    btnNext.Enabled = (currentPage < lastPageIndex)
End Sub

' Functie pentru prevenirea schimbarii manuale a paginilor
Public Sub PreventManualPageChange(MP As MSForms.MultiPage)
    If MP.Value <> currentPage Then
        MP.Value = currentPage
    End If
End Sub

' Functie pentru a sari direct la Page7 si a retine pagina anterioara
Public Sub JumpToLastPage(MP As MSForms.MultiPage, btnBack As MSForms.CommandButton, btnNext As MSForms.CommandButton)
    previousPage = currentPage
    
    ActivatePageByIndex MP, btnBack, btnNext, 7
End Sub

' Functie pentru a reveni la pagina anterioara de pe Page7
Public Sub ReturnFromLastPage(MP As MSForms.MultiPage, btnBack As MSForms.CommandButton, btnNext As MSForms.CommandButton)
    ActivatePageByIndex MP, btnBack, btnNext, previousPage
    
    MP.Pages(6).Enabled = False
End Sub

' OPTIMIZAT: Functie pentru validarea paginii curente
Public Function ValidateCurrentPage(frm As MSForms.UserForm, pageIndex As Integer) As Boolean
    Dim controlsToValidate As Variant
    Dim i As Integer, txtControl As String
    
    ValidateCurrentPage = True
    
    Select Case pageIndex
        Case 0: controlsToValidate = Array("TextBox1", "TextBox2")
        Case 1: controlsToValidate = Array("TextBox3")
        Case 2: controlsToValidate = Array("TextBox4", "TextBox5", "TextBox6", "TextBox7", "TextBox8")
        Case 3: controlsToValidate = Array("TextBox11", "TextBox12", "TextBox13", "TextBox14", "TextBox15")
        Case 4: controlsToValidate = Array("TextBox16", "TextBox17", "TextBox18")
        Case 5: controlsToValidate = Array("TextBox19", "TextBox20", "TextBox21", "TextBox22", "TextBox23", "TextBox24", "TextBox25")
    End Select
    
    For i = LBound(controlsToValidate) To UBound(controlsToValidate)
        txtControl = controlsToValidate(i)
        If Trim(frm.Controls(txtControl).Value) = "" Then
            MsgBox "Va rugam completati campul " & txtControl & " de pe pagina " & (pageIndex + 1) & "!", vbExclamation, "Validare"
            frm.Controls(txtControl).SetFocus
            ValidateCurrentPage = False
            Exit Function
        End If
    Next i
    
    ValidateCurrentPage = True
End Function

' Functie pentru adaugarea datelor in ListBox-uri
Public Sub AdaugaDateInListBox1(frm As MSForms.UserForm, ByVal sursaDate As String, Optional ByVal columnWidths As String)
    Dim newIndex As Integer
        
    Select Case sursaDate
        Case "CautareObiect"
            If frm.ListBox1.ColumnCount < 3 Then
                frm.ListBox1.ColumnCount = 3
                frm.ListBox1.columnWidths = columnWidths
            End If
            
            frm.ListBox1.AddItem ""
            newIndex = frm.ListBox1.ListCount - 1
            
            frm.ListBox1.List(newIndex, 0) = NextEmptyRowInListBox("ListBox1")
            frm.ListBox1.List(newIndex, 1) = frm.TextBox2.Text
            frm.ListBox1.List(newIndex, 2) = frm.TextBox1.Text

        Case "CautareLucrare"
            If frm.ListBox3.ColumnCount < 2 Then
                frm.ListBox3.ColumnCount = 2
                frm.ListBox3.columnWidths = columnWidths
            End If
            frm.ListBox3.AddItem ""
            newIndex = frm.ListBox3.ListCount - 1
            
            frm.ListBox3.List(newIndex, 0) = NextEmptyRowInListBox("ListBox3")
            frm.ListBox3.List(newIndex, 1) = frm.TextBox3.Text

        Case "CautareNorma"
            If frm.ListBox4.ColumnCount < 5 Then
                frm.ListBox4.ColumnCount = 5
                frm.ListBox4.columnWidths = columnWidths
            End If
            frm.ListBox4.AddItem ""
            newIndex = frm.ListBox4.ListCount - 1
            
            frm.ListBox4.List(newIndex, 0) = NextEmptyRowInListBox("ListBox4")
            frm.ListBox4.List(newIndex, 1) = frm.TextBox4.Text
            frm.ListBox4.List(newIndex, 2) = frm.TextBox5.Text
            frm.ListBox4.List(newIndex, 3) = frm.TextBox6.Text
            frm.ListBox4.List(newIndex, 4) = frm.TextBox8.Text
            
        Case "CautareMateriale"
            If frm.ListBox5.ColumnCount < 6 Then
                frm.ListBox5.ColumnCount = 6
                frm.ListBox5.columnWidths = columnWidths
            End If
            frm.ListBox5.AddItem ""
            newIndex = frm.ListBox5.ListCount - 1
        
            frm.ListBox5.List(newIndex, 0) = NextEmptyRowInListBox("ListBox5")
            frm.ListBox5.List(newIndex, 1) = frm.TextBox11.Text
            frm.ListBox5.List(newIndex, 2) = frm.TextBox12.Text
            frm.ListBox5.List(newIndex, 3) = frm.TextBox13.Text
            frm.ListBox5.List(newIndex, 4) = frm.TextBox14.Text
            frm.ListBox5.List(newIndex, 5) = frm.TextBox15.Text
                      
            frm.TextBox11.Text = ""
            frm.TextBox12.Text = ""
            frm.TextBox13.Text = ""
            frm.TextBox14.Text = ""
            frm.TextBox15.Text = ""
                      
        Case "CautareUtilaj"
            If frm.ListBox6.ColumnCount < 4 Then
                frm.ListBox6.ColumnCount = 4
                frm.ListBox6.columnWidths = columnWidths
            End If
            frm.ListBox6.AddItem ""
            newIndex = frm.ListBox6.ListCount - 1
        
            frm.ListBox6.List(newIndex, 0) = NextEmptyRowInListBox("ListBox6")
            frm.ListBox6.List(newIndex, 1) = frm.TextBox16.Text
            frm.ListBox6.List(newIndex, 2) = frm.TextBox17.Text
            frm.ListBox6.List(newIndex, 3) = frm.TextBox18.Text
            
            frm.TextBox16.Text = ""
            frm.TextBox17.Text = ""
            frm.TextBox18.Text = ""

        Case "CautareTransport"
            If frm.ListBox7.ColumnCount < 8 Then
                frm.ListBox7.ColumnCount = 8
                frm.ListBox7.columnWidths = columnWidths
            End If
            frm.ListBox7.AddItem ""
            newIndex = frm.ListBox7.ListCount - 1
        
            frm.ListBox7.List(newIndex, 0) = NextEmptyRowInListBox("ListBox7")
            frm.ListBox7.List(newIndex, 1) = frm.TextBox19.Text
            frm.ListBox7.List(newIndex, 2) = frm.TextBox20.Text
            frm.ListBox7.List(newIndex, 3) = frm.TextBox21.Text
            frm.ListBox7.List(newIndex, 4) = frm.TextBox22.Text
            frm.ListBox7.List(newIndex, 5) = frm.TextBox23.Text
            frm.ListBox7.List(newIndex, 6) = frm.TextBox24.Text
            frm.ListBox7.List(newIndex, 7) = frm.TextBox25.Text
            
            frm.TextBox19.Text = ""
            frm.TextBox20.Text = ""
            frm.TextBox21.Text = ""
            frm.TextBox22.Text = ""
            frm.TextBox23.Text = ""
            frm.TextBox24.Text = ""
            frm.TextBox25.Text = ""
        Case Else
            MsgBox "Sursa de date necunoscuta: " & sursaDate, vbExclamation
            Exit Sub
    End Select
End Sub

' OPTIMIZAT: Functie pentru gasirea urmatorului rand liber in ListBox
Function NextEmptyRowInListBox(listBoxName As String) As Long
    Dim lb As MSForms.listBox
    Dim i As Long, j As Long
    Dim isEmpty As Boolean
    Dim frm As MSForms.UserForm
    
    On Error Resume Next
    Set lb = ActiveSheet.OLEObjects(listBoxName).Object
    If Err.Number <> 0 Then
        For Each frm In UserForms
            On Error Resume Next
            Set lb = frm.Controls(listBoxName)
            If Not lb Is Nothing Then Exit For
        Next frm
    End If
    On Error GoTo 0
    
    If lb Is Nothing Then
        MsgBox "ListBox '" & listBoxName & "' not found!", vbExclamation
        NextEmptyRowInListBox = -1
        Exit Function
    End If
    
    If lb.ListCount = 0 Then
        NextEmptyRowInListBox = 1
        Exit Function
    End If
    
    ' Verificam mai intai doar prima coloana pentru un rand gol
    For i = 0 To lb.ListCount - 1
        If Trim(lb.List(i, 0)) = "" Then
            ' Verificam si restul coloanelor pentru siguranta
            isEmpty = True
            For j = 1 To lb.ColumnCount - 1
                If Trim(lb.List(i, j)) <> "" Then
                    isEmpty = False
                    Exit For
                End If
            Next j
            
            If isEmpty Then
                NextEmptyRowInListBox = i + 1
                Exit Function
            End If
        End If
    Next i
    
    NextEmptyRowInListBox = lb.ListCount + 1
End Function

' Functie pentru popularea ComboBox cu numele coloanelor
Public Sub PopuleazaComboBoxCuColoane(ByVal sheetName As String)
    Dim ws As Worksheet
    Dim i As Integer
    Dim ultimaColoana As Integer
    Dim comboBox As MSForms.comboBox
    
    Set comboBox = UserForm1.MultiPage1.Pages("Page8").Controls("ComboBox6")
    
    On Error Resume Next
    Set ws = wbBDFisaPV.Worksheets(sheetName)
    On Error GoTo 0

    If ws Is Nothing Then
        MsgBox "Foaia4 " & sheetName & " nu exista!", vbCritical
        Exit Sub
    End If

    comboBox.Clear

    ultimaColoana = ws.Cells(1, ws.Columns.Count).End(xlToLeft).Column
    
    For i = 1 To ultimaColoana
        comboBox.AddItem ws.Cells(1, i).Value
    Next i
    
    If comboBox.ListCount > 0 Then
        comboBox.ListIndex = 0
    End If
End Sub

' Functie optimizata pentru popularea ListBox2 cu date din foaia specificata
Public Sub PopuleazaListBoxCuDate(ByVal sheetName As String, Optional ByVal latimeColoane As String = "")
    Dim ws As Worksheet
    Dim listBox As MSForms.listBox
    Dim i As Long, j As Integer, k As Long
    Dim ultimaColoana As Integer
    Dim ultimulRand As Long
    Dim matriceDate As Variant
    Dim latimeMaxima() As Single
    
    Set listBox = UserForm1.MultiPage1.Pages("Page8").Controls("ListBox2")
    
    On Error Resume Next
    Set ws = wbBDFisaPV.Worksheets(sheetName)
    On Error GoTo 0

    If ws Is Nothing Then
        MsgBox "Foaia1 " & sheetName & " nu exista!", vbCritical
        Exit Sub
    End If
    
    ultimulRand = ws.Cells(ws.Rows.Count, 1).End(xlUp).Row
    ultimaColoana = ws.Cells(1, ws.Columns.Count).End(xlToLeft).Column

    If ultimulRand < 2 Or ultimaColoana < 1 Then
        MsgBox "Foaia '" & sheetName & "' nu contine date suficiente pentru a popula ListBox!", vbExclamation
        Exit Sub
    End If
    
    ReDim latimeMaxima(1 To ultimaColoana)
    
    matriceDate = ws.Range(ws.Cells(1, 1), ws.Cells(ultimulRand, ultimaColoana)).Value
    
    listBox.Clear
    
    listBox.ColumnCount = ultimaColoana
    
    Dim latimi As String
    
    If latimeColoane = "" Then
        For j = 1 To ultimaColoana
            latimeMaxima(j) = Len(CStr(matriceDate(1, j))) * 8 + 20
            
            For i = 2 To UBound(matriceDate, 1)
                Dim lungimeText As Single
                lungimeText = Len(CStr(matriceDate(i, j))) * 8 + 20
                If lungimeText > latimeMaxima(j) Then
                    latimeMaxima(j) = lungimeText
                End If
            Next i
        Next j
        
        For j = 1 To ultimaColoana
            If j > 1 Then latimi = latimi & ";"
            latimi = latimi & CStr(latimeMaxima(j)) & " pt"
        Next j
    Else
        Dim liniiLatime() As String
        liniiLatime = Split(latimeColoane, ";")
        
        For j = 1 To ultimaColoana
            If j > 1 Then latimi = latimi & ";"
            
            If j <= UBound(liniiLatime) + 1 Then
                latimi = latimi & liniiLatime(j - 1) & " pt"
            Else
                latimi = latimi & "100 pt"
            End If
        Next j
    End If
    
    listBox.columnWidths = latimi
    
    Dim numarRanduri As Long
    numarRanduri = ultimulRand - 1
    
    For i = 2 To ultimulRand
        listBox.AddItem ""
        k = i - 2
        
        For j = 1 To ultimaColoana
            listBox.List(k, j - 1) = CStr(matriceDate(i, j))
        Next j
    Next i
    
    listBox.Tag = sheetName & "|" & ultimaColoana
    
    Dim numeColoane As String
    For j = 1 To ultimaColoana
        If j > 1 Then numeColoane = numeColoane & "|"
        numeColoane = numeColoane & CStr(matriceDate(1, j))
    Next j
    
    If InStr(UserForm1.Tag, "CautareObiecte") > 0 Then
        UserForm1.Tag = "CautareObiecte|" & numeColoane
    ElseIf InStr(UserForm1.Tag, "CautareNorme") > 0 Then
        UserForm1.Tag = "CautareNorme|" & numeColoane
    End If
End Sub

' OPTIMIZAT: Functie pentru filtrarea in timp real a ListBox2
Public Sub FiltreazaListBoxInTimpReal(ByVal sheetName As String, ByVal Criteriu As String, ByVal Coloana As String)
    Static ultimCriteriu As String
    Static ultimaColoanaSelectata As String
    Static dateCacheCompleteMatrix As Variant
    
    Dim ws As Worksheet
    Dim i As Long, j As Integer, k As Long
    Dim ultimaColoana As Integer
    Dim ultimulRand As Long
    Dim indexColoana As Integer
    Dim listBox As MSForms.listBox
    Dim latimeColoane As String
    
    Set listBox = UserForm1.MultiPage1.Pages("Page8").Controls("ListBox2")
    
    If isEmpty(dateCacheCompleteMatrix) Or ultimaColoanaSelectata <> Coloana Then
        On Error Resume Next
        Set ws = wbBDFisaPV.Worksheets(sheetName)
        On Error GoTo 0
        
        If ws Is Nothing Then
            MsgBox "Foaia2 " & sheetName & " nu exista!", vbCritical
            Exit Sub
        End If
        
        ultimulRand = ws.Cells(ws.Rows.Count, 1).End(xlUp).Row
        ultimaColoana = ws.Cells(1, ws.Columns.Count).End(xlToLeft).Column
        
        dateCacheCompleteMatrix = ws.Range(ws.Cells(1, 1), ws.Cells(ultimulRand, ultimaColoana)).Value
        
        indexColoana = 0
        For j = 1 To ultimaColoana
            If CStr(dateCacheCompleteMatrix(1, j)) = Coloana Then
                indexColoana = j
                Exit For
            End If
        Next j
        
        If indexColoana = 0 Then
            MsgBox "Coloana selectata nu exista!", vbCritical
            Exit Sub
        End If
        
        ultimaColoanaSelectata = Coloana
    Else
        ultimulRand = UBound(dateCacheCompleteMatrix, 1)
        ultimaColoana = UBound(dateCacheCompleteMatrix, 2)
        
        indexColoana = 0
        For j = 1 To ultimaColoana
            If CStr(dateCacheCompleteMatrix(1, j)) = Coloana Then
                indexColoana = j
                Exit For
            End If
        Next j
    End If
    
    latimeColoane = listBox.columnWidths
    
    listBox.Clear
    
    listBox.ColumnCount = ultimaColoana
    
    If latimeColoane <> "" Then
        listBox.columnWidths = latimeColoane
    End If
    
    ' Optimizare: Folosim un Array pentru a stoca toate randurile filtrate inainte de a le adauga
    Dim filteredData() As Variant
    Dim filteredCount As Long
    Dim criteriu_lcase As String
    
    criteriu_lcase = LCase(Trim(Criteriu))
    filteredCount = 0
    
    ' Determinam numarul de randuri filtrate (daca criteriul e gol, toate randurile)
    If criteriu_lcase = "" Then
        ' Toate randurile exceptand antetul
        filteredCount = ultimulRand - 1
        ReDim filteredData(1 To filteredCount, 1 To ultimaColoana)
        
        ' Populam array-ul direct
        For i = 2 To ultimulRand
            For j = 1 To ultimaColoana
                filteredData(i - 1, j) = dateCacheCompleteMatrix(i, j)
            Next j
        Next i
    Else
        ' Numaram randurile care se potrivesc criteriului
        Dim tempArray() As Long
        ReDim tempArray(1 To ultimulRand - 1)
        
        For i = 2 To ultimulRand
            If InStr(1, LCase(CStr(dateCacheCompleteMatrix(i, indexColoana))), criteriu_lcase) > 0 Then
                filteredCount = filteredCount + 1
                tempArray(filteredCount) = i
            End If
        Next i
        
        ' Alocam doar spatiul necesar
        If filteredCount > 0 Then
            ReDim filteredData(1 To filteredCount, 1 To ultimaColoana)
            
            ' Populam array-ul cu randurile filtrate
            For i = 1 To filteredCount
                For j = 1 To ultimaColoana
                    filteredData(i, j) = dateCacheCompleteMatrix(tempArray(i), j)
                Next j
            Next i
        End If
    End If
    
    ' Adaugam randurile filtrate la ListBox intr-un singur pas
    If filteredCount > 0 Then
        ' Adaugam randuri goale in ListBox
        For i = 1 To filteredCount
            listBox.AddItem ""
        Next i
        
        ' Populam toate celulele
        For i = 1 To filteredCount
            For j = 1 To ultimaColoana
                listBox.List(i - 1, j - 1) = CStr(filteredData(i, j))
            Next j
        Next i
    End If
    
    ultimCriteriu = Criteriu
End Sub

