function v = write_uchar_binary (filename, data)

  %% usage: v = write_uchar_binary (filename, data)
  %%
  %%  write data to filename in 'uchar' format
  %%  and return the number of bytes writen

  f = fopen (filename, 'wb');
  if (f < 0)
    v = 0;
  else
    v = fwrite (f, data, 'uchar');
    fclose (f);
  end;
end
