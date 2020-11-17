VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "工作表1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = True
Public Sub WorkSheet_Activate()
    Call combobox_init
End Sub
Private Sub select_template_Change()
    If select_template.Value = "(重新整理...)" Then
        select_template.Value = ""
        Call combobox_init
    End If
    
End Sub
Function combobox_init()
    Dim template_folder As String
    
    template_folder = ThisWorkbook.Path & "\模板\"

    If Dir(template_folder, vbDirectory) = vbNullString Then
        MsgBox "找不到模板資料夾" & vbCrLf & "請確認路徑是否正確"
    Else
        If select_template.Value = "" Then
            Dim o_FSO As Object
            Dim o_folder As Object
            Dim o_file As Object
        
            Set o_FSO = CreateObject("Scripting.FileSystemObject")
        
            Set o_folder = o_FSO.GetFolder(template_folder)
            
            select_template.Clear
            For Each o_file In o_folder.Files
                If InStrRev(o_file.name, ".doc") Or InStrRev(o_file.name, ".docx") Then
                    select_template.AddItem (o_file.name)
                End If
            Next o_file
            select_template.AddItem ("(重新整理...)")
        End If
    End If
    
End Function
Sub Genarate()
    Dim row_count As Integer
    Dim main_sheet As Worksheet, dummy_sheet As Worksheet
    Dim name As String

    If select_template.Value = "" Then
        MsgBox "請選擇模板"
        Exit Sub
    End If
    
    '建立progess bar
    Load UserForm1
    UserForm1.StartUpPosition = 0
    UserForm1.Left = Application.Left + (0.5 * Application.Width) - (0.5 * UserForm1.Width)
    UserForm1.Top = Application.Top + (0.5 * Application.Height) - (0.5 * UserForm1.Height)
    UserForm1.Show 0
    
    '關閉提示音
    Application.ScreenUpdating = False
    Application.DisplayAlerts = False
    
    '原sheet
    Set main_sheet = ActiveSheet
    
    '取得sheet name
    name = ActiveSheet.name
    
    '計算列數
    row_count = Cells(rows.Count, 1).End(xlUp).row
    
    '建立sheet
    If Not add_sheet Then
        final
        Exit Sub
    End If
    
    'dummy sheet
    Set dummy_sheet = Sheets(name & "(合併文件)")
    'main_sheet.Activate
    
    '建立標題
    If Not init(dummy_sheet) Then
        final
        Exit Sub
    End If
    
    '生成資料
    If Not create_data(main_sheet, dummy_sheet, row_count) Then
        final
        Exit Sub
    End If
    
    '同房者
    If Not roommate(dummy_sheet) Then
        final
        Exit Sub
    End If
    
    '合併列印
    If Not do_print(dummy_sheet, name) Then
        final
        Exit Sub
    End If

    final
    
    '完成
    MsgBox "合併列印完成" & vbCrLf & "識別證_" & name & ".docx"
    
End Sub
Function update_bar(per As Integer)
    With UserForm1
        .percent.Caption = per & "%"
        .bar.Width = per * (.percent.Width - 6) / 100
        
        .Repaint
    End With
    
End Function
Function final()
    '開啟提示音
    Application.ScreenUpdating = True
    Application.DisplayAlerts = True
    
    '移除progress bar
    Unload UserForm1
    
    '刪除dummy
    Sheets(name & "(合併文件)").Delete
    
End Function
Function do_print(dummy As Worksheet, name As String) As Boolean
    Dim word_app As Object, word_doc As Object
    
    '如發生錯誤 則跳出警告 並終止程式
    On Error GoTo error_handler
    
    '開啟word object並隱藏
    Set word_app = CreateObject("Word.Application")
    word_app.Visible = False
    
    '開啟word
    Set word_doc = word_app.Documents.Open(ThisWorkbook.Path & "\模板\" & select_template.Value)
    word_doc.Activate
    
    '設定合併列印
    With word_doc.MailMerge
        .MainDocumentType = wdCatalog
        .OpenDataSource _
            name:=ThisWorkbook.FullName, _
            ConfirmConversions:=False, _
            ReadOnly:=False, _
            LinkToSource:=True, _
            AddToRecentFiles:=False, _
            PasswordDocument:="", _
            PasswordTemplate:="", _
            WritePasswordDocument:="", _
            WritePasswordTemplate:="", _
            Revert:=False, _
            Format:=wdOpenFormatAuto, _
            Connection:= _
                "Provider=Microsoft.ACE.OLEDB.12.0;User ID=Admin;Data Source=" & ThisWorkbook.FullName & ";Mode=Read;Extended Properties=""HDR=YES;IMEX=1;"";Jet OLEDB:System database="""";Jet OLEDB:Registry Path="""";Jet OLEDB:Engine Type=35" _
                , SQLStatement:="SELECT * FROM `'" & dummy.name & "$'`", SQLStatement1:="", _
            SubType:=wdMergeSubTypeAccess
        
        .Destination = wdSendToNewDocument
        .SuppressBlankLines = True
        
        With .DataSource
            .FirstRecord = wdDefaultFirstRecord
            .LastRecord = wdDefaultLastRecord
        End With
        
        .Execute True
    End With
    Call update_bar(80)

    '儲存合併後的word
    word_app.ActiveDocument.SaveAs ThisWorkbook.Path & "\識別證_" & name & ".docx"

    word_doc.Close savechanges:=wdDoNotSaveChanges
    Set word_doc = Nothing
    
    word_app.Quit
    Set word_app = Nothing
    
    do_print = True
    Call update_bar(100)
    Exit Function
    
error_handler:
    MsgBox "合併列印時發生錯誤"
    
    On Error Resume Next
    'word_doc.Close savechanges:=wdDoNotSaveChanges
    Set word_doc = Nothing
    
    'word_app.Quit
    Set word_app = Nothing
    On Error GoTo 0
    
    do_print = False
    
End Function
Function add_sheet() As Boolean
    Dim sheet_name As String, new_sheet_name As String
    
    '如發生錯誤 則跳出警告 並終止程式
    On Error GoTo error_handler
    
    '取得sheet name
    sheet_name = ActiveSheet.name
    new_sheet_name = sheet_name & "(合併文件)"
    
    '刪除已存在的sheet
    Application.DisplayAlerts = False
    On Error Resume Next
    Sheets(new_sheet_name).Delete
    Application.DisplayAlerts = True
    On Error GoTo 0
    
    '建立sheet
    Sheets.Add(, ActiveSheet).name = new_sheet_name
    
    '隱藏dummy
    Sheets(new_sheet_name).Visible = False
    
    add_sheet = True
    Call update_bar(10)
    Exit Function
    
error_handler:
    MsgBox "建立新表單時發生錯誤"
    add_sheet = False
    
End Function
Function init(sheet As Worksheet) As Boolean
    
    '如發生錯誤 則跳出警告 並終止程式
    On Error GoTo error_handler

    sheet.Cells(1, 1).Value = "編號"
    sheet.Cells(1, 2).Value = "車別"
    sheet.Cells(1, 3).Value = "分房"
    sheet.Cells(1, 4).Value = "餐桌"
    sheet.Cells(1, 5).Value = "姓名"
    sheet.Cells(1, 6).Value = "同房者"
    sheet.Cells(1, 7).Value = "領隊"
    sheet.Cells(1, 8).Value = "領隊台灣手機"
    sheet.Cells(1, 9).Value = "領隊大陸手機"
    sheet.Cells(1, 10).Value = "標題"
    
    init = True
    Call update_bar(20)
    Exit Function
    
error_handler:
    MsgBox "初始化新表單時發生錯誤"
    init = False

End Function
Function create_data(main As Worksheet, dummy As Worksheet, row_count As Integer) As Boolean
    Dim row As Integer, i As Integer
    
    '如發生錯誤 則跳出警告 並終止程式
    On Error GoTo error_handler
    
    For i = 1 To row_count
        If Not IsEmpty(main.Cells(i, 1)) And IsNumeric(main.Cells(i, 1).Value) Then
            row = Int(main.Cells(i, 1).Value) + 1
    
            '編號
            dummy.Cells(row, 1).Value = main.Cells(i, 1).Value
    
            '車別
            dummy.Cells(row, 2).Value = main.Cells(i, 2)
            
            '分房
            If IsEmpty(main.Cells(i, 3)) Then
                dummy.Cells(row, 3).Value = dummy.Cells(row - 1, 3)
            Else
                dummy.Cells(row, 3).Value = main.Cells(i, 3)
            End If
            
            '餐桌
            dummy.Cells(row, 4).Value = main.Cells(i, 4)
            dummy.Cells(row, 4).NumberFormat = "@"
            
            '姓名
            dummy.Cells(row, 5).Value = main.Cells(i, 5)
            
            '領隊
            dummy.Cells(row, 7).Value = main.Cells(2, 3)
            
            '領隊台灣手機
            dummy.Cells(row, 8).Value = main.Cells(2, 6)
            
            '領隊大陸手機
            dummy.Cells(row, 9).Value = main.Cells(3, 6)
            
            '標題
            dummy.Cells(row, 10).Value = main.Cells(1, 3)
        End If
        Call update_bar(20 + i / row_count * 20)
    Next i
    
    create_data = True
    Call update_bar(40)
    Exit Function
    
error_handler:
    MsgBox "填寫新表單時發生錯誤"
    create_data = False
    
End Function
Function roommate(sheet As Worksheet)
    Dim row_count As Integer, i As Integer, j As Integer

    '如發生錯誤 則跳出警告 並終止程式
    On Error GoTo error_handler

    '計算列數
    row_count = sheet.Cells(rows.Count, 1).End(xlUp).row
    
    '尋找同房者
    For i = 2 To row_count
        For j = 2 To row_count
            If j <> i And sheet.Cells(i, 3).Value = sheet.Cells(j, 3).Value Then
                sheet.Cells(i, 6).Value = sheet.Cells(j, 5).Value & " " & sheet.Cells(i, 6).Value
            End If
        Next j
        Call update_bar(40 + i / row_count * 20)
    Next i
    
    roommate = True
    Call update_bar(60)
    Exit Function
    
error_handler:
    MsgBox "尋找同房者時發生錯誤"
    roommate = False
    
End Function
