function v = write_float_binary (data, filename)

  %% usage: v = write_complex_binary (filename, data)
  %%
  %%  write data to filename in 'float' format
  %%  and return the number of bytes writen

  if ((m = nargchk (2,2,nargin)))
    usage (m);
  endif;

  f = fopen (filename, "wb");
  if (f < 0)
    v = 0;
  else
    v = fwrite (f, data, "float");
    fclose (f);
  endif;
end
