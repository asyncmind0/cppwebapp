try {
  POSTParser = new MPFD::Parser();
  POSTParser->SetTempDirForFileUpload("/tmp");
  POSTParser->SetMaxCollectedDataLength(20*1024);

  POSTParser->SetContentType(/* Here you know the Content-type:
                                string. And you pass it.*/);

  const int ReadBufferSize = 4 * 1024;

  char input[ReadBufferSize];

  do {
    // Imagine that you redirected std::cin to accept POST data.
    std::cin.read(input, ReadBufferSize);
    int read = std::cin.gcount();
    if (read) {
      POSTParser->AcceptSomeData(input, read);
    }

  } while (!std::cin.eof());

  // Now see what we have:
  std::map<std::string,MPFD::Field *> fields=p.GetFieldsMap();

  std::cout << "Have " << fields.size() << " fields\n\r";

  std::map<std::string,MPFD::Field *>::iterator it;
  for (it=fields.begin();it!=fields.end();it++) {
    if (fields[it->first]->GetType()==MPFD::Field::TextType) {
      std::cout<<"Got text field: ["<<it->first<<"], value: ["<< fields[it->first]->GetTextTypeContent() <<"]\n";
    } else {
      std::cout<<"Got file field: ["<<it->first<<"] Filename:["<<fields[it->first]->GetFileName()<<"] \n";
    }
  }
} catch (MPFD::Exception e) {
  // Parsing input error
  FinishConnectionProcessing();
  return false;
  }
