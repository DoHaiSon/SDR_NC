function v = write_complex_binary (filename, data)

  %% usage: v = write_complex_binary (filename, data)
  %%
  %%  write data to filename in 'complex' format
  %%  and return the number of bytes writen
   
    data_r = real(data);
    data_i = imag(data);
    data_sz = numel(data);
    
    data_float = zeros(2*data_sz,1);
    for idx=1:data_sz
        data_float(idx*2 - 1) = data_r(idx);
        data_float(idx*2) = data_i(idx);
    end
    
    f = fopen (filename, 'wb');
    if (f < 0)
        v = 0;
    else
        v = fwrite (f, data_float, 'float');
        fclose (f);
    end
end
